// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IHUD.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UIHUD : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class OVERCASTEDREVAMP_API IIHUD
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent,Category="HUD | Hotbar")
	void ToggleHotBar(const bool Visible);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent,Category="HUD | Inventory")
	void ToggleInventory(const bool Visible);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent,Category="HUD | Map")
	void ToggleMap(const bool Visible);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent,Category="HUD | MainMenu")
	void ShowMainMenu(const bool Visible);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent,Category="HUD | CraftMenu")
	void ToggleCraftMenu(const bool Visible);
};
