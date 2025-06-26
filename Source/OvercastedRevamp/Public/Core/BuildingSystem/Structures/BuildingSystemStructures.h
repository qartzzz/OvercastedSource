// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuildingSystemStructures.generated.h"

/**
 * 
 */
class OVERCASTEDREVAMP_API BuildingSystemStructures
{
public:
	BuildingSystemStructures();
	~BuildingSystemStructures();
};

UENUM(BlueprintType)
enum class EBlockType : uint8
{
	Wall,
	LowWall,
	Floor,
	Foundation,
};

UENUM(BlueprintType)
enum class EBlockMesh : uint8
{
	None,
	WoodenWall,
	WoodenDoorway,
	WoodenWindow,
	WoodenLowWall,
	WoodenFloor,
	WoodenFoundation,
};

UENUM(BlueprintType)
enum class EBlockRotation : uint8
{
	Zero UMETA(DisplayName = "0"),
	Ninety UMETA(DisplayName = "90"),
};

USTRUCT(BlueprintType)
struct FBuildingBlockParams : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxHP;
	
	FBuildingBlockParams() : MaxHP(0) {}
};