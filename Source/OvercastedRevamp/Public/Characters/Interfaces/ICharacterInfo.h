// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Equipment/EquipmentDataTypes.h"
#include "UObject/Interface.h"
#include "ICharacterInfo.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UICharacterInfo : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class OVERCASTEDREVAMP_API IICharacterInfo
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent,Category="CharacterInfo | Equipment")
	const FEquipmentProtection GetEquipmentProtection();
};
