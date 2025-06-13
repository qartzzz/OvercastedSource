// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Components/Inventory/InventoryComponent.h"

#include "Characters/Equipment/EquipmentDataTypes.h"
#include "Net/UnrealNetwork.h"

UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = false;
    DT_Items = ConstructorHelpers::FObjectFinder<UDataTable>(TEXT("/Game/Items/DT_Items.DT_Items")).Object;
	DT_Equipment = ConstructorHelpers::FObjectFinder<UDataTable>(TEXT("/Game/Core/EquipmentSystem/DT_Equipment.DT_Equipment")).Object;
	// ...
}


/**FIND CHANGED INDEXES*/


void UInventoryComponent::InventoryUpdated()
{
	TArray<int> ChangedSlots;
	for (int i = 0; i != Content.Items.Num(); i++)
	{
		if (LocalPreviousItems.IsValidIndex(i))
		{
			if (LocalPreviousItems[i] != Content.Items[i])
			{
				ChangedSlots.Add(i);
			} 
		}
		else
		{
			ChangedSlots.Add(i); 
		}
	}
    OnItemsChanged.Broadcast(ChangedSlots);
	LocalPreviousItems = Content.Items;
}

int UInventoryComponent::GetItemAmount(const FName& ItemID)
{
	int Amount = 0;
	
	for (const FSerializedInventorySlot& Slot : Content.Items)
	{
		 if (Slot.Slot.ItemID == ItemID)
		 {
			 Amount += Slot.Slot.Amount;
		 }
	}
	
	return Amount;
}

bool UInventoryComponent::EnoughForCraft(const TArray<FRecipeRow> RecipeRows, const int& ItemAmount)
{
	for (const FRecipeRow& Row : RecipeRows)
	{
		if (GetItemAmount(Row.ItemID) < Row.Amount )
		{
			return false;
		}
	}
	return true;
}

bool UInventoryComponent::RemoveItemsForCraft(const TArray<FRecipeRow> RecipeRows,const int& ItemAmount)
{
  if (EnoughForCraft(RecipeRows,ItemAmount))
  {
	  for (const FRecipeRow& Row : RecipeRows)
	  {
	  	  int Amount = Row.Amount;
	  	
		  for (FSerializedInventorySlot& Slot: Content.Items)
		  {
			  if (Slot.Slot.ItemID == Row.ItemID)
			  {
			  	  const int AmountToRemove = FMath::Clamp(Amount,0,Slot.Slot.Amount);
				  Slot.Slot.Amount -= AmountToRemove;
			  	  Content.MarkItemDirty(Slot);
			  	  Amount -= AmountToRemove;
			  }
		  }
	  }
  }
  else
  {
	  return false;
  }
	GetOwner()->ForceNetUpdate();
	return true;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwnerRole() == ROLE_Authority)
	{
		Content.Items = DefaultItems;
		Content.Items.SetNum(InventorySize);
		
		GenerateSlotsUniqueIDs();
		
		Content.MarkArrayDirty();
		
		OC_InventoryUpdated();
	}
	else
	{
		LocalPreviousItems.SetNum(InventorySize);
	}
}

void UInventoryComponent::OnRep_Content()
{
	OC_InventoryUpdated();
}


/** MAIN FUNCTIONS */


int UInventoryComponent::AddToStack(const FInventorySlot& NewItem,const int Index)
{
	const int AddQuantity = FMath::Clamp(NewItem.Amount,0,GetMaxStackSize(NewItem.ItemID) - Content.Items[Index].Slot.Amount);
	
	Content.Items[Index].Slot.Amount += AddQuantity;
	
	Content.MarkItemDirty(Content.Items[Index]);
	
	return NewItem.Amount - AddQuantity;
}

int UInventoryComponent::AddToEmptySlot(const FInventorySlot& NewItem,const int EmptySlotIndex)
{
	const int MaxStackSize = GetMaxStackSize(NewItem.ItemID);
	const int ClampedItemAmount = FMath::Clamp(NewItem.Amount,0,MaxStackSize);
	
	Content.Items[EmptySlotIndex].Slot = NewItem;
	Content.Items[EmptySlotIndex].Slot.Amount = ClampedItemAmount;
	
	if (DT_Items->FindRow<FItemData>(NewItem.ItemID,"")->ConstantItemInts.Contains(EConstantItemTags::ContentSlots))
	{
		Content.Items[EmptySlotIndex].SlotContent.SetNum(DT_Items->FindRow<FItemData>(NewItem.ItemID,"")->ConstantItemInts.FindRef(EConstantItemTags::ContentSlots));
	}
	
	Content.MarkItemDirty(Content.Items[EmptySlotIndex]);
	
	return NewItem.Amount - ClampedItemAmount;
}

void UInventoryComponent::GenerateSlotsUniqueIDs()
{
	TArray<uint8> UniqueIDs;
	for (int i = 0; i != Content.Items.Num(); i++)
	{
		uint8 NewSlotUniqueID = FMath::RandRange(0,255);
		
		while (UniqueIDs.Contains(NewSlotUniqueID))
		{
			NewSlotUniqueID = FMath::RandRange(0,255);
		}
		
		UniqueIDs.Add(NewSlotUniqueID);
		
		Content.Items[i].UniqueSlotID = NewSlotUniqueID;
	}
}

bool UInventoryComponent::AddItems(FInventorySlot NewItem)
{
	if (DT_Items->GetRowNames().Contains(NewItem.ItemID))
	{
		bool Failed = false;
	
		while(!Failed && NewItem.Amount)
		{
			const int StackIndex = FindStackableIndex(NewItem);
			const int EmptyIndex = FindEmptyIndex();
			if (StackIndex != -1)
			{
				//GEngine->AddOnScreenDebugMessage(-1,5,FColor::White,FString::FromInt(Content[StackIndex].Amount));
				//GEngine->AddOnScreenDebugMessage(-1,5,FColor::Yellow,FString::FromInt(StackIndex));
				NewItem.Amount = AddToStack(NewItem,StackIndex);  
			}
			else if(EmptyIndex != -1)
			{
				//GEngine->AddOnScreenDebugMessage(-1,5,FColor::Blue,FString::FromInt(Content[EmptyIndex].Amount));
				//GEngine->AddOnScreenDebugMessage(-1,5,FColor::Emerald,FString::FromInt(EmptyIndex));
				NewItem.Amount = AddToEmptySlot(NewItem,EmptyIndex);
			}
			else
			{
				Failed = true;
			}
		}
		GetOwner()->ForceNetUpdate();
		return NewItem.Amount == 0;
	}
	return false;
}


/** HELPER FUNCTIONS */


int UInventoryComponent::GetMaxStackSize(const FName ItemID) const
{
	if (DT_Items->GetRowNames().Contains(ItemID))
	{
		return DT_Items->FindRow<FItemData>(ItemID,"")->MaxStackSize;
	}
	return -1;
}

bool UInventoryComponent::IsInnerAcceptable(const FName Outer, const FName ItemID) const
{
	if (DT_Items->GetRowNames().Contains(Outer) && DT_Items->GetRowNames().Contains(ItemID))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, ItemID.ToString());
		return DT_Items->FindRow<FItemData>(Outer,"")->AcceptableInnerItems.Contains(ItemID);
	}
	return false;
}

int UInventoryComponent::FindEmptyIndex() const
{
	for (int i = 0 ; i != Content.Items.Num() ; i++)
	{
		if (Content.Items[i].Slot.Amount == 0)
		{
			return i;
		}
	}
	return -1;
}

int UInventoryComponent::FindStackableIndex(const FInventorySlot& ItemToStack) const 
{
	const int MaxStackSize = GetMaxStackSize(ItemToStack.ItemID);

	for (int i = 0 ; i != Content.Items.Num() ; i++)
	{
		if (Content.Items[i].Slot.ItemID == ItemToStack.ItemID && Content.Items[i].Slot.Amount != MaxStackSize && Content.Items[i].Slot.Amount > 0)
		{
			return i;
		}
	}
	return -1;
}


/** NETWORKING */


void UInventoryComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	FDoRepLifetimeParams ContentParms;
	ContentParms.Condition = COND_OwnerOnly;
	ContentParms.RepNotifyCondition = REPNOTIFY_OnChanged;
	DOREPLIFETIME_WITH_PARAMS(UInventoryComponent,Content,ContentParms)
	
}

void UInventoryComponent::OC_InventoryUpdated_Implementation()
{
	InventoryUpdated();
}

void UInventoryComponent::ServerMoveItem_Implementation(UInventoryComponent* DestinationInventory,const int DestinationIndex,const int SourceIndex ,const int AddAmount,const int SourceSlotContentIndex,const int DestinationSlotContentIndex)
{
	if (DestinationInventory->Features.Contains(EInventoryFeatures::ForEquipment))
	{
		if (DT_Equipment->GetRowNames().Contains(Content.Items[SourceIndex].Slot.ItemID))
		{
			GEngine->AddOnScreenDebugMessage(-1,5,FColor::Red,"Equipment has been added");
			//заменить вещи
			const FSerializedInventorySlot CachedSourceData = Content.Items[SourceIndex];
			Content.Items[SourceIndex] = DestinationInventory->Content.Items[DestinationIndex];
			DestinationInventory->Content.Items[DestinationIndex] = CachedSourceData;

			Content.MarkItemDirty(Content.Items[SourceIndex]);
			DestinationInventory->Content.MarkItemDirty(DestinationInventory->Content.Items[DestinationIndex]);

			//найти конфликтующие вещи
			for (FSerializedInventorySlot& Slot : DestinationInventory->Content.Items)
			{
				//состоит ли слот в дата тейбле снаряжения
				if (DT_Equipment->GetRowNames().Contains(Slot.Slot.ItemID) && Slot != DestinationInventory->Content.Items[DestinationIndex])
				{
					//итерация через все слоты брони
					for (const EEquipmentBodyPart& BodyPart : DT_Equipment->FindRow<FEquipmentData>(Slot.Slot.ItemID,"")->OccupiedBodyParts)
					{
						//сравнение занятых элементов
						if (DT_Equipment->FindRow<FEquipmentData>(DestinationInventory->Content.Items[DestinationIndex].Slot.ItemID,"")->OccupiedBodyParts.Contains(BodyPart))
						{
							AddItems(Slot.Slot);
							Slot = FSerializedInventorySlot();
							DestinationInventory->Content.MarkItemDirty(Slot);
						}
					}
				}
			}
		}
		
		return;
	}
	
	const FSerializedInventorySlot CachedSourceData = Content.Items[SourceIndex];
	const FSerializedInventorySlot CachedDestinationData = DestinationInventory->Content.Items[DestinationIndex];
	
	FSerializedInventorySlot &DestinationData = DestinationInventory->Content.Items[DestinationIndex];
	FSerializedInventorySlot &SourceData = Content.Items[SourceIndex];
	
	const int MaxStackSize = GetMaxStackSize(SourceData.Slot.ItemID);
	
	if (SourceSlotContentIndex == -1 && DestinationSlotContentIndex == -1)
	{
		const int ClampedAddAmount = FMath::Clamp(FMath::Clamp(AddAmount,0,SourceData.Slot.Amount),0,MaxStackSize - DestinationData.Slot.Amount);
		
		if (SourceData.Slot.ItemID == DestinationData.Slot.ItemID)
		{
			DestinationData.Slot.Amount += ClampedAddAmount;
			SourceData.Slot.Amount -= ClampedAddAmount;
		}
		else if (DestinationData.Slot.ItemID == "")
		{
			DestinationData = SourceData;
			DestinationData.Slot.Amount = ClampedAddAmount;
			DestinationData.SlotContent = SourceData.SlotContent;
			
			SourceData.SlotContent.Empty();
			SourceData.Slot.Amount -= ClampedAddAmount;

			DestinationData.UniqueSlotID = SourceData.UniqueSlotID;
			SourceData.UniqueSlotID = CachedDestinationData.UniqueSlotID;	
		}
		else
		{
			SourceData = DestinationData;
			DestinationData = CachedSourceData;
		}
	}
	else if (SourceSlotContentIndex != -1 && DestinationSlotContentIndex == -1)
	{
		FInventorySlot &InnerSourceData = Content.Items[SourceIndex].SlotContent[SourceSlotContentIndex];

		const int ClampedAddAmount = FMath::Clamp(FMath::Clamp(AddAmount,0,InnerSourceData.Amount),0,MaxStackSize - DestinationData.Slot.Amount);
		
		if (InnerSourceData.ItemID == DestinationData.Slot.ItemID)
		{
			DestinationData.Slot.Amount += ClampedAddAmount;
			InnerSourceData.Amount -= ClampedAddAmount;
		}
		else if (DestinationData.Slot.ItemID == "")
		{
			DestinationData.Slot = InnerSourceData;
			DestinationData.Slot.Amount = ClampedAddAmount;
			
			InnerSourceData.Amount -= ClampedAddAmount;
		}

		if (InnerSourceData.Amount <= 0)
		{
			InnerSourceData = FInventorySlot();
		}
	}
	else if (SourceSlotContentIndex == -1 && DestinationSlotContentIndex != -1 && IsInnerAcceptable(DestinationData.Slot.ItemID,SourceData.Slot.ItemID))
	{
		FInventorySlot &InnerDestinationData = DestinationData.SlotContent[DestinationSlotContentIndex];
		
		if (InnerDestinationData.ItemID == "")
		{
			InnerDestinationData = SourceData.Slot;
			InnerDestinationData.Amount = AddAmount;
			
			SourceData.Slot.Amount -= AddAmount;
		}
		else if (InnerDestinationData.ItemID == SourceData.Slot.ItemID)
		{
			InnerDestinationData.Amount += AddAmount;
			SourceData.Slot.Amount -= AddAmount;
		}
		else
		{
			SourceData.Slot = InnerDestinationData;
			InnerDestinationData = CachedSourceData.Slot;
		}
	}
	else if (SourceSlotContentIndex != -1 && DestinationSlotContentIndex != -1)
	{
		FInventorySlot &InnerDestinationData = DestinationData.SlotContent[DestinationSlotContentIndex];
		FInventorySlot &SourceSlotData = SourceData.SlotContent[SourceSlotContentIndex];

		if (InnerDestinationData.ItemID == "")
		{
			InnerDestinationData = SourceSlotData;
			InnerDestinationData.Amount = AddAmount;
			
			SourceSlotData.Amount -= AddAmount;

			if (SourceSlotData.Amount <= 0)
			{
				SourceSlotData = FInventorySlot();
			}
		}
		else if (InnerDestinationData.ItemID == SourceSlotData.ItemID)
		{
			InnerDestinationData.Amount += AddAmount;
			SourceSlotData.Amount -= AddAmount;
		}
		else
		{
			const FInventorySlot LocalSlotData = InnerDestinationData;
			InnerDestinationData = SourceSlotData;
			SourceSlotData = LocalSlotData;
		}
	}
	
	if (SourceData.Slot.Amount <= 0)
	{
		SourceData.Slot = FInventorySlot();
		SourceData.SlotContent.Empty();
	}
	if (DestinationData.Slot.Amount <= 0)
	{
		DestinationData.Slot = FInventorySlot();
		DestinationData.SlotContent.Empty();
	}
	
	Content.MarkItemDirty(SourceData);
	DestinationInventory->Content.MarkItemDirty(DestinationData);
	
	GetOwner()->ForceNetUpdate();
}
