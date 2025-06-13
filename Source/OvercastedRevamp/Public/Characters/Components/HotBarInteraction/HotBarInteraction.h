// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/PlayerCharacter.h"
#include "Components/ActorComponent.h"
#include "Interfaces/IWeapon.h"
#include "HotBarInteraction.generated.h"


class UInventoryComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class OVERCASTEDREVAMP_API UHotBarInteraction : public UActorComponent , public IIWeapon
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UHotBarInteraction();

	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	
	uint8 CurrentSlot = 255;

	uint8 CurrentSlotUniqueID = 255;
	
	UPROPERTY()
	UDataTable* DT_Items;
	
	UPROPERTY()
	UInventoryComponent* OwnerInventory = nullptr;
	
	UPROPERTY()
	UActorComponent* CurrentWeapon = nullptr;

	UPROPERTY()
	APlayerCharacter* OwningCharacter = nullptr;
	
	TSharedPtr<FStreamableHandle> StreamableHandle;
	UFUNCTION(Server,Unreliable)
	void ChangeWeapon(const uint8 SlotIndex);

	void DestroyWeapon() const;

	void OnComponentLoaded();
};
