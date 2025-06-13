// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IPlayerAbilities.h"
#include "GameFramework/GameMode.h"
#include "GamemodeSurvival.generated.h"

/**
 * 
 */
UCLASS()
class OVERCASTEDREVAMP_API AGamemodeSurvival : public AGameMode , public IIPlayerAbilities
{
	GENERATED_BODY()

public:
	AGamemodeSurvival();
	
    virtual void PostLogin(APlayerController* NewPlayer) override;

    void Respawn_Implementation(APlayerController* OwningController) override;

protected:
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Gamemode | Respawn")
    void CalculateRespawnPoint(APlayerController* OwningController);

};
