// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IPlayerAbilities.h"
#include "GameFramework/PlayerController.h"
#include "PlayerControllerBase.generated.h"

/**
 * 
 */
UCLASS()
class OVERCASTEDREVAMP_API APlayerControllerBase : public APlayerController, public IIPlayerAbilities
{
	GENERATED_BODY()
//constructor
public:
    APlayerControllerBase();



//Public overrides
public:
virtual void Tick(float DeltaSeconds) override;





//Protected overrides
protected: 
	virtual void BeginPlay() override;


protected:
	

//Private overrides
private: 
	void Respawn_Implementation(APlayerController* PlayerController) override;

	
    



//Private variables
private: 
	TSubclassOf<UUserWidget> RespawnMenu = nullptr;





//Private UFUNCTIONs
private:
    UFUNCTION(Server, Unreliable)
    void Server_Respawn();
};
