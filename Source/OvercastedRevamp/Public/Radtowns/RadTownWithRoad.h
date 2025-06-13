// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RadTownBase.h"
#include "RadTownWithRoad.generated.h"

UCLASS()
class OVERCASTEDREVAMP_API ARadTownWithRoad : public ARadTownBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARadTownWithRoad();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	USceneComponent* RoadIn;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	USceneComponent* RoadOut;
private:
	
	UPROPERTY(EditDefaultsOnly,meta = (AllowPrivateAccess = "true"))
	USceneComponent* Root;
};
