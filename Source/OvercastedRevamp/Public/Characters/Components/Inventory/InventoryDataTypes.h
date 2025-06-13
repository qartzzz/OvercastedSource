// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "InventoryDataTypes.generated.h"
UENUM(BlueprintType)
enum class EItemAnimations : uint8
{
	None UMETA(DisplayName = "None"),
	MP5 UMETA(DisplayName = "MP5"),
};
UENUM(BlueprintType)
enum class EContainerTags : uint8
{
	RottingSpeed UMETA(DisplayName = "RottingSpeed"),
};

UENUM(BlueprintType)
enum class EItemTags : uint8
{
	None UMETA(DisplayName = "None"),
	Durability UMETA(DisplayName = "Durability"),
	Rotting UMETA(DisplayName = "Rotting"),
	Ammunition UMETA(DisplayName = "Ammunition"),
};

USTRUCT(BlueprintType)
struct FItemTags
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory",meta = (DisplayName = "Key", MakeStructureDefaultValue = "None"))
	EItemTags Key;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory",meta = (DisplayName = "Amount", MakeStructureDefaultValue = "0"))
    FName Value;
	FItemTags(EItemTags Key = EItemTags::None, FName Value = "") : Key(EItemTags::None), Value("") {}

	bool operator==(const FItemTags& Data) const
	{
		return Data.Key == Key && Data.Value == Value;
	}

	bool operator!=(const FItemTags& Data) const
	{
		return Data.Key != Key || Data.Value != Value;
	}
};
//



UENUM(BlueprintType)
enum class EConstantItemTags : uint8
{
	None UMETA(DisplayName = "None"),
	Modifiable UMETA(DisplayName = "Modifiable"),
	Modular UMETA(DisplayName = "Modular"),
	Decayable UMETA(DisplayName = "Decayable"),
	MaxDurability UMETA(DisplayName = "Max durability"),
	Firearms UMETA(DisplayName = "Firearms"),
	Melee UMETA(DisplayName = "Melee"),
	Edible UMETA(DisplayName = "Edible"),
	ContentSlots UMETA(DisplayName = "ContentSlots"),
};

UENUM(BlueprintType)
enum class EItemParameters : uint8
{
	None UMETA(DisplayName = "None"),
	Damage UMETA(DisplayName = "Damage"),
	Recoil UMETA(DisplayName = "Recoil"),
	AimCone UMETA(DisplayName = "Spread"),
	FireRate UMETA(DisplayName = "Firerate"),
	
	Range UMETA(DisplayName = "Range"),
	MeleeFireRate UMETA(DisplayName = "Melee firerate"),
	MeleeRange UMETA(DisplayName = "Melee range"),
	
	Calories UMETA(DisplayName = "Calories"),
	Water UMETA(DisplayName = "Water"),
	Heal UMETA(DisplayName = "Heal"),
	Poison UMETA(DisplayName = "Poison"),
	
	Protection UMETA(DisplayName = "Protection"),
	ColdProtection UMETA(DisplayName = "Cold protection"),
	RadiationProtection UMETA(DisplayName = "Radiation protection"),
	MeleeProtection UMETA(DisplayName = "Melee protection"),
};

USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
	
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FName ID;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FText DisplayName;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FText Description;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,meta = (MakeStructureDefaultValue = "x"))
	FName AmountPrefix;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	int MaxStackSize;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TMap<EConstantItemTags,FName> ConstantItemTags;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TMap<EConstantItemTags,int> ConstantItemInts;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TMap<EConstantItemTags,bool> ConstantItemBools;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TMap<EItemParameters,float> ItemParameters;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TArray<FName> AcceptableInnerItems;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> Icon;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TSoftClassPtr<UActorComponent> WeaponClass;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	EItemAnimations ItemAnimation;

	FItemData() : ID(""),DisplayName(FText::FromString(TEXT(""))),Description(FText::FromString(TEXT(""))),AmountPrefix("x"),MaxStackSize(0),ConstantItemTags(),ConstantItemInts(),ConstantItemBools(),Icon(nullptr),WeaponClass(nullptr),ItemAnimation(EItemAnimations::None){}
};
UCLASS()
class OVERCASTEDREVAMP_API UInventoryDataTypes : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable,Category="Inventory | ItemTags",BlueprintPure)
	static FName FindItemTag(TArray<FItemTags> Tags , EItemTags Key);
	UFUNCTION(BlueprintCallable,Category="Inventory | ItemTags",BlueprintPure)
	static bool DoesItemHaveTag(TArray<FItemTags> Tags,EItemTags Key);
};
