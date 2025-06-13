//Copyright OVERCASTED, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IScreenVFX.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UIScreenVFX : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class OVERCASTEDREVAMP_API IIScreenVFX
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ScreenVFX | Interaction")
	void HealthChanged(float PreviousHP, float CurrentHP, FVector CauserLocation);
};
