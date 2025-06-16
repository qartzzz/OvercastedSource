// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IDamageTypeInfo.generated.h"

UENUM(Blueprintable)
enum class EDamageTypeTags: uint8
{
	CanFarmWood UMETA(DisplayName = "Can farm wood"),
	CanFarmNodes UMETA(DisplayName = "Can farm nodes"),
	
};
// This class does not need to be modified.
UINTERFACE()
class UIDamageTypeInfo : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class OVERCASTEDREVAMP_API IIDamageTypeInfo
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent,Category="DamageTypeInfo")
	bool HaveDamageTypeTag(const EDamageTypeTags Tag);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent,Category="DamageTypeInfo")
	const TArray<EDamageTypeTags> GetDamageTypeTags();
};
