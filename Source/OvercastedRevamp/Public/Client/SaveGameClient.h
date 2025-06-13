// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveGameClient.generated.h"

/**
 * 
 */
UCLASS()
class OVERCASTEDREVAMP_API USaveGameClient : public USaveGame
{
	GENERATED_BODY()
private:
	TArray<uint8> SavedMapAlpha;
	TArray<uint8> SavedMapPaintings;
	TMap<FName,FName> UserSettings;
};
