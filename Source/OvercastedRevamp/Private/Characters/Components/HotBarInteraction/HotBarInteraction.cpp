// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Components/HotBarInteraction/HotBarInteraction.h"
#include "Characters/Components/Inventory/InventoryComponent.h"
#include "Engine/AssetManager.h"



// Sets default values for this component's properties
UHotBarInteraction::UHotBarInteraction()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	DT_Items = ConstructorHelpers::FObjectFinder<UDataTable>(TEXT("/Game/Items/DT_Items.DT_Items")).Object;
	// ...
}


// Called when the game starts
void UHotBarInteraction::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwnerRole() == ROLE_Authority)
	{
		OwningCharacter = Cast<APlayerCharacter>(GetOwner());
		OwnerInventory = Cast<UInventoryComponent>(OwningCharacter->GetComponentByClass(UInventoryComponent::StaticClass()));
		
	}
}

void UHotBarInteraction::ChangeWeapon_Implementation(const uint8 SlotIndex)
{
	DestroyWeapon();
	
	if (OwnerInventory->Content.Items.IsValidIndex(SlotIndex))
	{
		CurrentSlot = SlotIndex;
		CurrentSlotUniqueID = OwnerInventory->Content.Items[SlotIndex].UniqueSlotID;
		const FSerializedInventorySlot Item = OwnerInventory->Content.Items[SlotIndex];
		if (DT_Items->GetRowNames().Contains(Item.Slot.ItemID))
		{
			const TSoftClassPtr<UActorComponent> WeaponClass = DT_Items->FindRow<FItemData>(Item.Slot.ItemID,"")->WeaponClass;
			FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
			FStreamableDelegate Delegate;
			Delegate.BindUObject(this,&ThisClass::OnComponentLoaded);
			StreamableHandle = StreamableManager.RequestAsyncLoad(WeaponClass.ToSoftObjectPath(),Delegate);
			//DT_Items->FindRow<FItemData>(Item.Slot.ItemID,"")->WeaponClass;
			OwningCharacter->SetCurrentAnimation(DT_Items->FindRow<FItemData>(Item.Slot.ItemID,"")->ItemAnimation);
		}
	}
	else
	{
		OwningCharacter->SetCurrentAnimation(EItemAnimations::None);
	}
}

void UHotBarInteraction::OnComponentLoaded()
{
	if (StreamableHandle.IsValid())
	{
		CurrentWeapon = OwningCharacter->AddComponentByClass(Cast<UClass>(StreamableHandle->GetLoadedAsset()),false,FTransform::Identity,false);
	}
}

void UHotBarInteraction::DestroyWeapon() const
{
	if (IsValid(CurrentWeapon))
	{
		CurrentWeapon->DestroyComponent(); 
		OwningCharacter->SetCurrentAnimation(EItemAnimations::None);
	}
}

