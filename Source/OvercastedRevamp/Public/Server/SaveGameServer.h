//Copyright OVERCASTED, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "ISaveGameServer.h"
#include "SaveGameServer.generated.h"

/**
 * 
 */
UCLASS()
class OVERCASTEDREVAMP_API USaveGameServer : public USaveGame , public IISaveGameServer
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadWrite)
	int64 WipeTime = 0;
private:
	int64 SetWipeTime_Implementation() override;

	int64 GetWipeTime_Implementation() override;
};
