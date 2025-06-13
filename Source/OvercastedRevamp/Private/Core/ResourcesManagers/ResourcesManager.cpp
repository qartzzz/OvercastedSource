// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/ResourcesManager/ResourcesManager.h"

// Sets default values
AResourcesManager::AResourcesManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AResourcesManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AResourcesManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

