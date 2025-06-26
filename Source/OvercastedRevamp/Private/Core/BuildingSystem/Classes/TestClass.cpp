// Fill out your copyright notice in the Description page of Project Settings.


#include "TestClass.h"


// Sets default values
ATestClass::ATestClass()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	HISM = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>("HISM");
}

// Called when the game starts or when spawned
void ATestClass::BeginPlay()
{
	Super::BeginPlay();
	for (int i = 0; i != 5000; i++)
	{
		InstancesVectors.Add(FVector(FMath::RandRange(-3000,3000), FMath::RandRange(-3000,3000), 0));
	}
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle,this,&ATestClass::RemoveAndAddInstances,10,true);
}

// Called every frame
void ATestClass::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATestClass::RemoveAndAddInstances()
{
	const int NumInstances = HISM->GetNumInstances();
	TArray<FVector> InstancesTransforms;
	
	for (int i = 0; i != NumInstances; i++)
	{
		FTransform OutTransform;
		HISM->GetInstanceTransform(i,OutTransform,false);
		InstancesTransforms.Add(OutTransform.GetLocation());
	}

	for (const FVector& Vector : InstancesVectors)
	{
		if (!InstancesTransforms.Contains(Vector))
		{
			HISM->AddInstance(FTransform(FRotator(),Vector,FVector(1,1,1)),false);
		}
	}
	
}

 