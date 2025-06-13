// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExpensesVolume.generated.h"

class USphereComponent;
enum class EExpenses : uint8;

UCLASS()
class OVERCASTEDREVAMP_API AExpensesVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExpensesVolume();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	virtual void OnConstruction(const FTransform& Transform) override;
	UPROPERTY(EditAnywhere)
    float AddPerSecond;
	UPROPERTY(EditAnywhere)
	float TickRate = 1;
	UPROPERTY(EditAnywhere)
	EExpenses Expense;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float InnerRadius = 100;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,meta = (ClampMin = 0))
	float Falloff = 100;
private:
	UFUNCTION()
	void ExpenseTick();
	UPROPERTY(EditDefaultsOnly,meta = (AllowPrivateAccess=true))
	USphereComponent* OuterSphere;
	UPROPERTY(EditDefaultsOnly,meta = (AllowPrivateAccess=true))
	USphereComponent* InnerSphere;
	UPROPERTY(EditDefaultsOnly)
	USceneComponent* Root;
	
};
