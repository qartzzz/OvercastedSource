// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/DamageTypes/DamageTypeBase.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Projectile.generated.h"

UCLASS()
class OVERCASTEDREVAMP_API AProjectile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditDefaultsOnly)
	float ProjectileRadius;

	UPROPERTY(EditDefaultsOnly)
	float DamageAmount;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTypeBase> DamageType;
private:
	UPROPERTY(EditDefaultsOnly,meta = (AllowPrivateAccess=true))
	UProjectileMovementComponent* ProjectMovementComponent;

	bool WasHit;

	void ProjectileTick(float DeltaTime);
	
    UPROPERTY()
	APawn* OwnerPawn;
	
	UPROPERTY()
	UDataTable* DT_Impacts;
};
