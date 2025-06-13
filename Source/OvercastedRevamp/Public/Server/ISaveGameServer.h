//Copyright OVERCASTED, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ISaveGameServer.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UISaveGameServer : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class OVERCASTEDREVAMP_API IISaveGameServer
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	//Returns server wipe time in unix
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DedicatedServer | Server statistics")
	int64 GetWipeTime();
	//Sets server wipe time in unix
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DedicatedServer | Server statistics")
	int64 SetWipeTime();
};
