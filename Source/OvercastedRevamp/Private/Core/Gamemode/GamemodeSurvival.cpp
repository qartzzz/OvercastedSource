// Fill out your copyright notice in the Description page of Project Settings.
#include "Core/Gamemode/GamemodeSurvival.h"
#include "Core/Gamemode/RespawnSystem/RespawnPoint.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/HUD.h"

AGamemodeSurvival::AGamemodeSurvival()
{
	HUDClass = ConstructorHelpers::FClassFinder<AHUD>(TEXT("/Game/Character/UI_HUD/BP_HUD")).Class;
	DefaultPawnClass = ConstructorHelpers::FClassFinder<APawn>(TEXT("/Game/Character/BP_PlayerCharacter")).Class;
    PlayerControllerClass = ConstructorHelpers::FClassFinder<APlayerController>(TEXT("/Game/Character/PlayerController/BP_PlayerController")).Class;
	PrimaryActorTick.bCanEverTick = false;
}

void AGamemodeSurvival::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	if (NewPlayer->GetPawn())
	{
		NewPlayer->GetPawn()->Destroy();
	}
}

void AGamemodeSurvival::Respawn_Implementation(APlayerController* OwningController)
{
	if (OwningController->GetPawn())
	{
		OwningController->GetPawn()->Destroy();
	}
	
	CalculateRespawnPoint(OwningController);
}

void AGamemodeSurvival::CalculateRespawnPoint(APlayerController* OwningController)
{
	if (IsValid(OwningController))
	{
		TArray<AActor*> Points;

		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARespawnPoint::StaticClass(), Points);
		
		GEngine->AddOnScreenDebugMessage(-1,5,FColor::Red,"ControllerValid");
		for (int i = 0;i != 100;i++)
		{
			const AActor* Actor = Points[FMath::RandRange(0,Points.Num() - 1)];
			TArray<FHitResult> HitResults;
			GetWorld()->LineTraceMultiByChannel(HitResults, Actor->GetActorLocation(), Actor->GetActorLocation() - FVector(0, 0, 550),ECollisionChannel::ECC_Visibility);
			for (FHitResult& HitResult : HitResults)
			{
				if (HitResult.bBlockingHit)
				{
					FActorSpawnParameters SpawnInfo;
					APawn* RespawnedPawn = GetWorld()->SpawnActor<APawn>(DefaultPawnClass,FTransform(FRotator::ZeroRotator,Actor->GetActorLocation(),FVector(1,1,1)), SpawnInfo);
					GEngine->AddOnScreenDebugMessage(-1,5,FColor::Red,"Respawned");
					OwningController->Possess(RespawnedPawn);
					return;
				}
			}
			
		}
	}
}
