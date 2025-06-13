// Fill out your copyright notice in the Description page of Project Settings.


#include "Radtowns/RadTownBase.h"

// Sets default values
ARadTownBase::ARadTownBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	

}

// Called when the game starts or when spawned
void ARadTownBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARadTownBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

