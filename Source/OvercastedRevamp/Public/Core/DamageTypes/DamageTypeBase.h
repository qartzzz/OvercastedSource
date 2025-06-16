//Copyright OVERCASTED, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "Interfaces/IDamageTypeInfo.h"
#include "DamageTypeBase.generated.h"

/**
 * 
 */


UENUM(Blueprintable)
enum class EObjectTypes : uint8
{
	WoodenHard UMETA(DisplayName = "WoodenHard"),
	WoodenMedium UMETA(DisplayName = "WoodenMedium"),
	WoodenWeak UMETA(DisplayName = "WoodenWeak"),
	StoneHard UMETA(DisplayName = "StoneHard"),
	StoneMedium UMETA(DisplayName = "StoneMedium"),
	StoneWeak UMETA(DisplayName = "StoneWeak"),
	MetalHard UMETA(DisplayName = "MetalHard"),
	MetalMedium UMETA(DisplayName = "MetalMedium"),
	MetalWeak UMETA(DisplayName = "MetalWeak"),
	PawnHard UMETA(DisplayName = "PawnHard"),
	PawnMedium UMETA(DisplayName = "PawnMedium"),
	PawnWeak UMETA(DisplayName = "PawnWeak"),
};

UCLASS()
class OVERCASTEDREVAMP_API UDamageTypeBase : public UDamageType , public IIDamageTypeInfo
{
	GENERATED_BODY()
	
public:
	TArray<EDamageTypeTags> Tags;
	
	TMap<EObjectTypes, float> DamageMultipliers;

	float GetDamageMultiplier(const EObjectTypes& ObjectType);

	bool HaveDamageTypeTag_Implementation(const EDamageTypeTags Tag) override;

	const TArray<EDamageTypeTags> GetDamageTypeTags_Implementation() override;
};
