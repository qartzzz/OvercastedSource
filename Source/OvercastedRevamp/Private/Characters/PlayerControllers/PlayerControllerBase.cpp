// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PlayerControllers/PlayerControllerBase.h"
#include "UMG.h"



APlayerControllerBase::APlayerControllerBase()
{
	RespawnMenu = ConstructorHelpers::FClassFinder<UUserWidget>(TEXT("/Game/Character/RespawnMenu/WBP_RespawnMenu")).Class;
}

void APlayerControllerBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void APlayerControllerBase::Respawn_Implementation(APlayerController* PlayerController)
{
	Server_Respawn();
}


void APlayerControllerBase::Server_Respawn_Implementation()
{
	if (GetWorld()->GetAuthGameMode()->GetClass()->ImplementsInterface(UIPlayerAbilities::StaticClass()))
	{
		IIPlayerAbilities::Execute_Respawn(GetWorld()->GetAuthGameMode(),this);
	}
}


void APlayerControllerBase::BeginPlay()
{
	Super::BeginPlay();
	if (IsLocalPlayerController())
	{
		CreateWidget(this, RespawnMenu)->AddToViewport();
	}
}

