// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/DamageTypes/DamageTypesData.h"
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

UENUM(BlueprintType)
enum class EBodyPart : uint8
{
	None,
	Head,
	Body,
	Legs,
};

USTRUCT(BlueprintType)
struct FEquipmentProtection
{
	GENERATED_BODY()

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

	FEquipmentProtection operator+(const FEquipmentProtection& rhs)
	{
		FirearmProtection += rhs.FirearmProtection;
		MeleeProtection += rhs.MeleeProtection;
		AnimalsProtection += rhs.AnimalsProtection;
		RadiationProtection += rhs.RadiationProtection;
		ColdProtection += rhs.ColdProtection;
		HeatProtection += rhs.HeatProtection;
		
		return *this;
	}

	float GetTypeProtection(const EDamageType DamageType) const
	{
		switch (DamageType) {
		case EDamageType::FirearmDamage:
			return FirearmProtection;
			break;
		case EDamageType::MeleeDamage:
			return MeleeProtection;
			break;
		case EDamageType::AnimalDamage:
			return AnimalsProtection;
			break;
		}
	    return 0.0f;
	}
	FEquipmentProtection(const float InFirearmProtection = 0,const float InMeleeProtection = 0,const float InAnimalsProtection = 0,const float InRadiationProtection = 0,const float InColdProtection = 0,const float InHeatProtection = 0) : FirearmProtection(InFirearmProtection), MeleeProtection(InMeleeProtection), AnimalsProtection(InAnimalsProtection),RadiationProtection(InRadiationProtection), ColdProtection(InColdProtection), HeatProtection(InHeatProtection) {}
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
	TArray<EEquipmentBodyPart> OccupiedEquipmentBodyParts;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EBodyPart> OccupiedBodyParts;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEquipmentProtection Protection;

	FEquipmentData() : OccupiedEquipmentBodyParts(TArray<EEquipmentBodyPart>()), Protection(FEquipmentProtection()) {}
};