// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/CraftingSystem/WorkbenchComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UWorkbenchComponent::UWorkbenchComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
	DT_ItemCrafts = ConstructorHelpers::FObjectFinder<UDataTable>(TEXT("/Game/Core/CraftingSystem/DT_ItemCraft.DT_ItemCraft")).Object;
}


// Called when the game starts
void UWorkbenchComponent::BeginPlay()
{
	GetOwner()->ForceNetUpdate();
	Super::BeginPlay();
	OwnerInventory = Cast<UInventoryComponent>(GetOwner()->GetComponentByClass(UInventoryComponent::StaticClass()));
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, OwnerInventory->GetName());
}

void UWorkbenchComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams QueueReplicationParms;
	QueueReplicationParms.Condition = COND_OwnerOnly;
	QueueReplicationParms.RepNotifyCondition = REPNOTIFY_OnChanged;
	DOREPLIFETIME_WITH_PARAMS(UWorkbenchComponent,CraftingQueue,QueueReplicationParms)
}

bool UWorkbenchComponent::AddCraftToQueue(const FName& ItemID,const int& Amount)
{
	if (OwnerInventory && DT_ItemCrafts->GetRowNames().Contains(ItemID) && OwnerInventory->EnoughForCraft(DT_ItemCrafts->FindRow<FItemCraftData>(ItemID,"")->RecipeRows,Amount))
	{
		SR_AddCraftToQueue(ItemID,Amount);
		return true;
	}
	return false;
}

void UWorkbenchComponent::SR_AddCraftToQueue_Implementation(const FName ItemID,const int Amount)
{
	if (OwnerInventory->EnoughForCraft(DT_ItemCrafts->FindRow<FItemCraftData>(ItemID,"")->RecipeRows,Amount))
	{
		OwnerInventory->RemoveItemsForCraft(DT_ItemCrafts->FindRow<FItemCraftData>(ItemID,"")->RecipeRows,Amount);
		
		const float CraftTime = DT_ItemCrafts->FindRow<FItemCraftData>(ItemID,"")->CraftTime;
	
		CraftingQueue.CraftingQueue.Add(FCraftingQueueSlot(ItemID,Amount,CraftTime));
		CraftingQueue.MarkItemDirty(CraftingQueue.CraftingQueue.Last());
	
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		
		if (!TimerManager.TimerExists(TimerHandle))
		{
			TimerManager.SetTimer(TimerHandle,this,&ThisClass::CraftTick,TickRate,true);
		}
	}
		
}

bool UWorkbenchComponent::RemoveCraftByIndex(const int& Index)
{
	if (CraftingQueue.CraftingQueue.IsValidIndex(Index))
	{
		SR_RemoveCraftByIndex(Index);
		return true;
	}
	return false;
}

void UWorkbenchComponent::SR_RemoveCraftByIndex_Implementation(const int& Index)
{
	if (CraftingQueue.CraftingQueue.IsValidIndex(Index))
	{
		const FCraftingQueueSlot& QueueSlot = CraftingQueue.CraftingQueue[Index];
		for (const FRecipeRow& Row : DT_ItemCrafts->FindRow<FItemCraftData>(QueueSlot.ItemID,"")->RecipeRows)
		{
			FInventorySlot ItemToAdd;
			ItemToAdd.ItemID = Row.ItemID;
			ItemToAdd.Amount = Row.Amount*QueueSlot.Amount;
			OwnerInventory->AddItem(ItemToAdd);
		}

		CraftingQueue.CraftingQueue.RemoveAt(Index);
		CraftingQueue.MarkArrayDirty();
	}
}

bool UWorkbenchComponent::RemoveCraftByName(const FName& ItemID)
{
	for (const FCraftingQueueSlot& Slot : CraftingQueue.CraftingQueue)
	{
		if (Slot.ItemID == ItemID)
		{
			SR_RemoveCraftByName(ItemID);
			return true;
		}
	}
	return false;
}

void UWorkbenchComponent::SR_RemoveCraftByName_Implementation(const FName& ItemID)
{
	TArray<FCraftingQueueSlot>& CachedCraftingQueue = CraftingQueue.CraftingQueue;
	for (int i = 0; i != CachedCraftingQueue.Num(); i++)
	{
		if (CachedCraftingQueue.IsValidIndex(i))
		{
			const FCraftingQueueSlot& Slot = CachedCraftingQueue[i];
			if (Slot.ItemID == ItemID)
			{
				for (const FRecipeRow& Row : DT_ItemCrafts->FindRow<FItemCraftData>(Slot.ItemID,"")->RecipeRows)
				{
					FInventorySlot ItemToAdd;
					ItemToAdd.ItemID = Row.ItemID;
					ItemToAdd.Amount = Row.Amount*Slot.Amount;
					OwnerInventory->AddItem(ItemToAdd);
				}
				
				CraftingQueue.CraftingQueue.RemoveAt(i);
				CraftingQueue.MarkArrayDirty();

				return;
			}
		}
	}
}

void UWorkbenchComponent::CraftTick()
{
	if (!CraftingQueue.CraftingQueue.IsValidIndex(0))
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		return;
	}
	
	FCraftingQueueSlot& CraftingQueueSlot = CraftingQueue.CraftingQueue[0];
	
	CraftingQueueSlot.TimeRemaining -= TickRate;
	CraftingQueue.MarkItemDirty(CraftingQueueSlot);
	
    if (CraftingQueueSlot.TimeRemaining <= 0 && GetOwner()->HasAuthority())
    {
    	CraftingQueueSlot.Amount--;
    	
    	const int AmountToAdd = DT_ItemCrafts->FindRow<FItemCraftData>(CraftingQueueSlot.ItemID,"")->Amount;
    	
    	FInventorySlot Slot;
    	Slot.ItemID = CraftingQueueSlot.ItemID;
    	Slot.Amount = AmountToAdd;
    	OwnerInventory->AddItem(Slot);
    	
    	if (CraftingQueueSlot.Amount == 0)
    	{
    		CraftingQueue.CraftingQueue.RemoveAt(0);
    		CraftingQueue.MarkArrayDirty();
    	}

    	if (CraftingQueue.CraftingQueue.IsEmpty())
    	{
    		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
    	}
    }
}
