// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/WildlifeManager/WildlifeManager.h"
#include "Engine/World.h"
#include "NavigationSystem.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"


// Sets default values
AWildlifeManager::AWildlifeManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	bReplicates = true;
	bAlwaysRelevant = true;
	
	SetNetDormancy(ENetDormancy::DORM_Initial);
	SetNetUpdateFrequency(0);
	
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickInterval(150);
}

void AWildlifeManager::BeginPlay()
{
	Super::BeginPlay();
}

void AWildlifeManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (HasAuthority())
	{
		TArray<TObjectPtr<APlayerState>> OnlinePlayers = GetWorld()->GetGameState()->PlayerArray;
		const int OnlinePlayersCount =  OnlinePlayers.Num();

		for (const APlayerState* OnlinePlayer : OnlinePlayers)
		{
			if (OnlinePlayer->GetPawn())
			{
				TrySpawnWildlife(OnlinePlayer->GetPawn()->GetActorLocation() + OnlinePlayer->GetPawn()->GetActorForwardVector() * 3800,OnlinePlayersCount);
			}
		} 
	}
}

bool AWildlifeManager::TrySpawnWildlife(const FVector& PlayerLocation,const int PlayersOnline)
{
	if (FMath::RandRange(0,20) > FMath::GetMappedRangeValueClamped(FVector2D(0,600),FVector2D(5,1),PlayersOnline))
	{
		FHitResult Hit;
		
  	    GetWorld()->LineTraceSingleByChannel(Hit,PlayerLocation+FVector(0,0,30000),PlayerLocation-FVector(0,0,60000),ECC_WorldStatic);
		
		FNavLocation NavLocation;
		const UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
		
		if (NavSystem->GetRandomPointInNavigableRadius(Hit.Location,2500,NavLocation))
		{
			for (int i = 0;i != 3;i++)
			{
				GetWorld()->SpawnActor<APawn>(AnimalClasses[FMath::RandRange(0,AnimalClasses.Num() - 1)],NavLocation.Location + FVector(FMath::RandRange(-20.f,20.f),FMath::RandRange(-20.f,20.f),0),FRotator(0.0f));
			}
		}
	}
	return false;
}

