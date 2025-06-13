// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Components/Expenses/ExpenseAffects/ExpenseAffectRadiation.h"

#include "Kismet/GameplayStatics.h"

UExpenseAffectRadiation::UExpenseAffectRadiation()
{
	RadiationSound = ConstructorHelpers::FObjectFinder<USoundBase>(TEXT("/Game/Core/ExpensesComponent/SFX/Radiation/Geiger-SFX-Low.Geiger-SFX-Low")).Object;
}

void UExpenseAffectRadiation::Affect(AActor* TargetActor,const float& ExpenseValue)
{
	if (!TargetActor->HasAuthority() && ExpenseValue > FMath::RandRange(0,100))
	{
		UGameplayStatics::PlaySound2D(TargetActor,RadiationSound);
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "ExpenseAffectRadiation");
	}
	
	if (TargetActor->HasAuthority() && ExpenseValue > 30)
	{
		const FDamageEvent DamageEvent;
		TargetActor->TakeDamage(FMath::GetMappedRangeValueClamped(FVector2D(30,500),FVector2D(2,5),ExpenseValue),DamageEvent,nullptr,nullptr);
	}
}
