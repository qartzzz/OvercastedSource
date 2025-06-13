// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IWeapon.generated.h"
UENUM(BlueprintType)
enum class EWeaponAction : uint8
{
	Attack UMETA(DisplayName="Attack"),
	Attack2 UMETA(DisplayName="Attack2"),
	Reload UMETA(DisplayName="Reload"),
};
// This class does not need to be modified.
UINTERFACE()
class UIWeapon : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class OVERCASTEDREVAMP_API IIWeapon
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon | Actions")
	void PerformAction(EWeaponAction Action);
};
 