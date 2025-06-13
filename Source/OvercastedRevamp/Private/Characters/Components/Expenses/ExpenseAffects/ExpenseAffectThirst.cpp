// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Components/Expenses/ExpenseAffects/ExpenseAffectThirst.h"

void UExpenseAffectThirst::Affect(AActor* TargetActor,const float& ExpenseValue)
{
	if (ExpenseValue < 100)
	{
		const FDamageEvent DamageEvent;
		TargetActor->TakeDamage(FMath::GetMappedRangeValueClamped(FVector2D(0,50),FVector2D(0.2,0.05),ExpenseValue),DamageEvent,nullptr,nullptr);
	}
}
