// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EquipmentDataTypes.generated.h"
/**
 * 
 */
class OVERCASTEDREVAMP_API EquipmentDataTypes
{
public:
	EquipmentDataTypes();
	~EquipmentDataTypes();
};



UENUM(BlueprintType)
enum class EEquipmentBodyPart : uint8
{
	None,
	Head,
	HeadOuter,
	Body,
	BodyOuter,
	Hands,
	Legs,
	LegsOuter,
	Feet
};

USTRUCT(BlueprintType)
struct OVERCASTEDREVAMP_API FEquipmentData : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<USkeletalMesh> SkeletalMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSoftObjectPtr<USkeletalMesh> FPSSkeletalMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EEquipmentBodyPart> OccupiedBodyParts;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FirearmProtection;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MeleeProtection;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AnimalsProtection;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RadiationProtection;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ColdProtection;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HeatProtection;

	FEquipmentData() : OccupiedBodyParts(TArray<EEquipmentBodyPart>()), FirearmProtection(0), MeleeProtection(0),AnimalsProtection(0),RadiationProtection(0),ColdProtection(0),HeatProtection(0) {}
};