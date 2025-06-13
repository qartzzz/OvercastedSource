// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IGameInstance.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UIGameInstance : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class OVERCASTEDREVAMP_API IIGameInstance
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent,Category="GameInstance|Inventory")
	bool ToggleFavorite(FName ItemID);
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent,Category="GameInstance|Inventory")
	TArray<FName> GetFavoriteItems();
};
