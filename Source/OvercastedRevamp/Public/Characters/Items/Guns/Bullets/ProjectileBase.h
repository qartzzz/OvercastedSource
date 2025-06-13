// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/Actor.h"
#include "Core/DamageTypes/DamageTypeBase.h"
#include "ProjectileBase.generated.h"

USTRUCT(BlueprintType)
struct FProjectileImpacts : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	USoundBase* ImpactSound;
	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactParticle;
	UPROPERTY(EditAnywhere)
	UMaterialInterface* ImpactMaterial;
	UPROPERTY(EditAnywhere)
	FVector DecalSize;
	FProjectileImpacts(USoundBase* ImpactSound = nullptr,UParticleSystem* ImpactParticle = nullptr,UMaterialInterface* ImpactMaterial = nullptr,FVector DecalSize = FVector::ZeroVector) : ImpactSound(), ImpactParticle(), ImpactMaterial(),DecalSize(FVector::ZeroVector) {}
};

UCLASS()
class OVERCASTEDREVAMP_API AProjectileBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProjectileBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly)
	float FalloffPower = 1;
	
	UPROPERTY(EditDefaultsOnly)
    float Speed = 5000;

	UPROPERTY(EditDefaultsOnly)
	float MinSpeed = 2500;
	
	UPROPERTY(EditDefaultsOnly)
	float MaxSpeed = 5000;
	
	UPROPERTY(EditDefaultsOnly)
	float DamageAmount = 35;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTypeBase> DamageType;
	
	UPROPERTY(EditDefaultsOnly)
	float ProjectileRadius = 2;
	
	FDamageEvent DamageEvent;

	bool WasHit;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
private:
	void MoveAndTrace(const float DeltaTime);
	UPROPERTY()
	UDataTable* DT_Impacts;
};
