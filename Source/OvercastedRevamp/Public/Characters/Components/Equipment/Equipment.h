// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Components/Inventory/InventoryComponent.h"
#include "Components/ActorComponent.h"
#include "Characters/Equipment/EquipmentDataTypes.h"
#include "Engine/StreamableManager.h"
#include "Equipment.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class OVERCASTEDREVAMP_API UEquipment : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UEquipment();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintReadOnly)
	TMap<EBodyPart,FEquipmentProtection> EquipmentProtection;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
private:
	UPROPERTY(ReplicatedUsing=OnRep_Equipment)
	TArray<FName> CurrentEquipment;
	
	UPROPERTY()
	UInventoryComponent* OwnerEquipment;

	UPROPERTY()
	UDataTable* DT_Equipment;

	TSharedPtr<FStreamableHandle> EquipmentLoadHandle;

	TSharedPtr<FStreamableHandle> FPSEquipmentLoadHandle;
	UFUNCTION()
	void OnRep_Equipment();
	UFUNCTION()
	void OnEquipmentLoaded();

	UFUNCTION()
	void OnFPSEquipmentLoaded();
	
	UFUNCTION()
	void UpdateEquipment(const TArray<int>& Indices);
};
