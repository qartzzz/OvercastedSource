// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Components/Expenses/ExpensesVolumes/ExpensesVolume.h"
#include "Characters/Components/Expenses/ExpensesComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
AExpensesVolume::AExpensesVolume()
{
	PrimaryActorTick.bCanEverTick = false;
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);
	OuterSphere = CreateDefaultSubobject<USphereComponent>(FName("OuterRadius"));
	OuterSphere->SetupAttachment(Root);
	InnerSphere = CreateDefaultSubobject<USphereComponent>("InnerRadius");
	InnerSphere->SetupAttachment(Root);
}

void AExpensesVolume::BeginPlay()
{
	Super::BeginPlay();
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle,this,&ThisClass::ExpenseTick,TickRate,true);
}

void AExpensesVolume::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	InnerSphere->SetSphereRadius(InnerRadius);
	OuterSphere->SetSphereRadius(InnerRadius + Falloff);
}

void AExpensesVolume::ExpenseTick()
{
	TArray<AActor*> Overlaps;
	OuterSphere->GetOverlappingActors(Overlaps,APawn::StaticClass());
	  
	for (AActor* Actor : Overlaps)
	{
		UActorComponent* Component = Actor->GetComponentByClass(UExpensesComponent::StaticClass());
		if (IsValid(Component)) 
		{
			Cast<UExpensesComponent>(Component)->AddExpense(Expense,(AddPerSecond * TickRate) * FMath::Lerp(1,0,FMath::Clamp((FVector::Distance(GetActorLocation(),Actor->GetActorLocation()) - InnerRadius) / Falloff,0,1)));
		}
	}
}



