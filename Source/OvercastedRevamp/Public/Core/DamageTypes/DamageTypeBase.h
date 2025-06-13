//Copyright OVERCASTED, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
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
class OVERCASTEDREVAMP_API UDamageTypeBase : public UDamageType
{
	GENERATED_BODY()
	
public:
	TMap<EObjectTypes, float> DamageMultipliers;

	float GetDamageMultiplier(const EObjectTypes& ObjectType);
};
