// Fill out your copyright notice in the Description page of Project Settings.


#include "Radtowns/RadTownWithRoad.h"


// Sets default values
ARadTownWithRoad::ARadTownWithRoad()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	Root = CreateDefaultSubobject<USceneComponent>("Root Component");
	SetRootComponent(Root);
	Root->SetComponentTickEnabled(false);
	RoadIn = CreateDefaultSubobject<USceneComponent>("RoadIn");
	RoadIn->SetupAttachment(Root);
	
	RoadOut = CreateDefaultSubobject<USceneComponent>("RoadOut");
	RoadOut->SetupAttachment(Root);

	RoadIn->SetMobility(EComponentMobility::Static);
	RoadOut->SetMobility(EComponentMobility::Static);
	
	RoadOut->SetComponentTickEnabled(false);
	RoadIn->SetComponentTickEnabled(false);
}

// Called when the game starts or when spawned
void ARadTownWithRoad::BeginPlay()
{
	Super::BeginPlay();
	
}


