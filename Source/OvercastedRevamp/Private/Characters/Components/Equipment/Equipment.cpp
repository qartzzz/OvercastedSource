// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Components/Equipment/Equipment.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Net/UnrealNetwork.h"
#include "SkeletalMeshUserData/BodyPartUserData.h"


UEquipment::UEquipment()
{
	SetIsReplicatedByDefault(true);
	
	PrimaryComponentTick.bCanEverTick = false;

	DT_Equipment = ConstructorHelpers::FObjectFinder<UDataTable>(TEXT("/Game/Core/EquipmentSystem/DT_Equipment.DT_Equipment")).Object;
	OwnerEquipment = nullptr;

	
}

void UEquipment::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass,CurrentEquipment);
}

void UEquipment::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwner()->HasAuthority())
	{
		OwnerEquipment = Cast<UInventoryComponent>(GetOwner()->GetComponentsByTag(UInventoryComponent::StaticClass(),FName("Equipment"))[0]);
		OwnerEquipment->OnItemsChanged.AddDynamic(this,&ThisClass::UpdateEquipment);
	}
}

void UEquipment::OnRep_Equipment()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Updating Equipment");
	EquipmentProtection.FindOrAdd(EBodyPart::Body,FEquipmentProtection());
	EquipmentProtection.FindOrAdd(EBodyPart::Head,FEquipmentProtection());
	EquipmentProtection.FindOrAdd(EBodyPart::Legs,FEquipmentProtection());

	TArray<FSoftObjectPath> SoftSkeletalMeshesPaths;
	TArray<FSoftObjectPath> SoftFPSSkeletalMeshesPaths;
	
	for (const FName& Slot : CurrentEquipment)
	{
		if (DT_Equipment->GetRowNames().Contains(Slot))
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Iterating slots");
			const FEquipmentData* EquipmentData = DT_Equipment->FindRow<FEquipmentData>(Slot,"");
			for (const EBodyPart& BodyPart : EquipmentData->OccupiedBodyParts)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, UEnum::GetValueAsString(BodyPart));
				EquipmentProtection.Add(BodyPart,EquipmentProtection.FindRef(BodyPart) + EquipmentData->Protection);
			}

			if (!EquipmentData->SkeletalMesh.IsNull())
			{
				GEngine->AddOnScreenDebugMessage(-1,5,FColor::Red,"SkeletalMeshSoftPtrValid");
				SoftSkeletalMeshesPaths.Add(EquipmentData->SkeletalMesh.ToSoftObjectPath());
			}
			
			if (!EquipmentData->FPSSkeletalMesh.IsNull())
			{
				GEngine->AddOnScreenDebugMessage(-1,5,FColor::Blue,"FPSSkeletalMeshSoftPtrValid");
				SoftFPSSkeletalMeshesPaths.Add(EquipmentData->FPSSkeletalMesh.ToSoftObjectPath());
			}
		}
	}

	FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
	
	if (!SoftSkeletalMeshesPaths.IsEmpty())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Starting async load");
		FStreamableDelegate Delegate;
		Delegate.BindUObject(this,&ThisClass::OnEquipmentLoaded);
		EquipmentLoadHandle = StreamableManager.RequestAsyncLoad(SoftSkeletalMeshesPaths, Delegate);
	}
	if (!SoftFPSSkeletalMeshesPaths.IsEmpty())
	{
		GEngine->AddOnScreenDebugMessage(-1,5,FColor::Blue,"Starting async load");
		FStreamableDelegate FPSDelegate;
		FPSDelegate.BindUObject(this,&ThisClass::OnFPSEquipmentLoaded);
		FPSEquipmentLoadHandle = StreamableManager.RequestAsyncLoad(SoftFPSSkeletalMeshesPaths, FPSDelegate);
	}
}

void UEquipment::UpdateEquipment(const TArray<int>& Indices) 
{
	CurrentEquipment.Empty();
	for (const FSerializedInventorySlot& Slot : OwnerEquipment->Content.Items)
	{
		if (Slot.Slot.Amount > 0)
		{
			CurrentEquipment.Add(Slot.Slot.ItemID);
		}
	}
	GetOwner()->ForceNetUpdate();
}



void UEquipment::OnEquipmentLoaded()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Completed async load");
	TArray<UObject*> LoadedObjects;
	const TArray<UActorComponent*> LoadedComponents = GetOwner()->GetComponentsByTag(USkeletalMeshComponent::StaticClass(),FName("SkeletalMesh"));
	
	EquipmentLoadHandle->GetLoadedAssets(LoadedObjects);
	
	if (!LoadedObjects.IsEmpty())
	{
		for (UActorComponent* LoadedComponent : LoadedComponents)
		{
			Cast<USkeletalMeshComponent>(LoadedComponent)->SetSkeletalMeshAsset(nullptr);
		}
	
		for (int i = 0; i != LoadedObjects.Num(); i++)
		{
			Cast<USkeletalMeshComponent>(LoadedComponents[i])->SetSkeletalMeshAsset(Cast<USkeletalMesh>(LoadedObjects[i]));
		}
	}
	
}

void UEquipment::OnFPSEquipmentLoaded()
{
	TArray<UObject*> LoadedObjects;
	FPSEquipmentLoadHandle->GetLoadedAssets(LoadedObjects);

	GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Blue,FString::FromInt(LoadedObjects.Num()));
	
	if (!LoadedObjects.IsEmpty())
	{
		GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Blue,"LoadedObjectsNotEmpty");
		for (UObject* ArmorPart : LoadedObjects)
		{
			USkeletalMesh* SkeletalMesh = Cast<USkeletalMesh>(ArmorPart);
			const UBodyPartUserData* Data = SkeletalMesh->GetAssetUserData<UBodyPartUserData>();
			GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Blue,"FPSEquipmentLoaded");
			GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Blue,UEnum::GetValueAsString(Data->BodyPart).RightChop(20));
			TArray<UActorComponent*> SkeletalMeshComponents = GetOwner()->GetComponentsByTag(USkeletalMeshComponent::StaticClass(),FName(UEnum::GetValueAsString(Data->BodyPart).RightChop(20)));
			
			for (UActorComponent* Component : SkeletalMeshComponents)
			{
				Cast<USkeletalMeshComponent>(Component)->SetSkeletalMesh(SkeletalMesh);
			}
		}
	}
}
