// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WildlifeManager.generated.h"

UCLASS()
class OVERCASTEDREVAMP_API AWildlifeManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWildlifeManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
    UPROPERTY(EditDefaultsOnly,Category="Wildlife")
	TArray<TSubclassOf<APawn>> AnimalClasses;
	
private:
	bool TrySpawnWildlife(const FVector& PlayerLocation,const int PlayersOnline);
};
