// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/CraftingSystem/CraftSystemStructures.h"
#include "Characters/Components/Inventory/InventoryComponent.h"
#include "WorkbenchComponent.generated.h"



UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class OVERCASTEDREVAMP_API UWorkbenchComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UWorkbenchComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
public:
	
	UFUNCTION(BlueprintCallable,Category="Workbench")
	bool AddCraftToQueue(const FName& ItemID,const int& Amount);

	UFUNCTION(BlueprintCallable,Category="Workbench")
	bool RemoveCraftByIndex(const int& Index);

	UFUNCTION(BlueprintCallable,Category="Workbench")
	bool RemoveCraftByName(const FName& ItemID);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite,Category="Workbench parameters")
	EWorkbenchType WorkbenchType;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite,Category="Workbench parameters")
	uint8 WorkbenchLevel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite,Category="Workbench parameters")
	float TickRate = 0.1;
	
	UPROPERTY(Replicated,BlueprintReadOnly,Category="Workbench|Crafting queue")
	FCraftingQueue CraftingQueue;

private:
	UFUNCTION(Server,Unreliable,Category="Workbench|Server")
	void SR_AddCraftToQueue(const FName ItemID,const int Amount);

	UFUNCTION(Server,Unreliable,Category="Workbench|Server")
	void SR_RemoveCraftByIndex(const int& Index);

	UFUNCTION(Server,Unreliable,Category="Workbench|Server")
	void SR_RemoveCraftByName(const FName& ItemID);
	
	FTimerHandle TimerHandle;
	
	UPROPERTY()
	UInventoryComponent* OwnerInventory;

	UPROPERTY()
	UDataTable* DT_ItemCrafts;

	UPROPERTY()
	UDataTable* DT_ItemData;
	
	UFUNCTION()
	void CraftTick();
};
