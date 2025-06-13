// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "CraftSystemStructures.generated.h"

/**
 * 
 */
class OVERCASTEDREVAMP_API CraftSystemStructures
{
public:
	CraftSystemStructures();
	~CraftSystemStructures();

	
};

UENUM(BlueprintType)
enum class EItemCraftCategory : uint8
{
	None,
	Favorites,
	Basic,
	Food,
	Tools,
	Weapons,  
	Building,
	Home,
	Equipment,
	
};

UENUM(BlueprintType)
enum class EWorkbenchType : uint8
{
	None,
	Player,
	Workbench,
	Furnace,
	Armory,
};

USTRUCT(BlueprintType)
struct FRecipeRow
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	FName ItemID;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	int Amount;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	bool NeedToBeFullyRepaired;

	FRecipeRow() : ItemID(""), Amount(0), NeedToBeFullyRepaired(false) {}
};

USTRUCT(BlueprintType)
struct FItemCraftData : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	TArray<FRecipeRow> RecipeRows;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	float CraftTime;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	int Amount;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	uint8 WorkbenchLevel;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	EWorkbenchType TableType;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	EItemCraftCategory Category;

	FItemCraftData() : RecipeRows({}),CraftTime(0),Amount(0),WorkbenchLevel(0), TableType(EWorkbenchType::None), Category(EItemCraftCategory::None) {}
};

USTRUCT(BlueprintType)
struct FCraftingQueueSlot : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FName ItemID;
	
	UPROPERTY(BlueprintReadWrite)
	int Amount;
	
	UPROPERTY(BlueprintReadWrite)
	float TimeRemaining;

	FCraftingQueueSlot(const FName& InItemID = "",const int& InAmount = 0,const float& InTimeRemaining = 0) : ItemID(InItemID), Amount(InAmount), TimeRemaining(InTimeRemaining) {}
};

USTRUCT(BlueprintType)
struct FCraftingQueue : public FFastArraySerializer
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite)
	TArray<FCraftingQueueSlot> CraftingQueue;
	
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FCraftingQueueSlot,FCraftingQueue>(CraftingQueue, DeltaParms,*this);
	}
};
