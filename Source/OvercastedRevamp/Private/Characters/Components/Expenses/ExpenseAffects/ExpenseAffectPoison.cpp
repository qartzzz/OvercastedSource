// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Components/Expenses/ExpenseAffects/ExpenseAffectPoison.h"

void UExpenseAffectPoison::Affect(AActor* TargetActor,const float& ExpenseValue)
{
	if (ExpenseValue > 1)
	{
		const FDamageEvent DamageEvent;
		TargetActor->TakeDamage(FMath::GetMappedRangeValueClamped(FVector2D(0,20),FVector2D(0.25,5),ExpenseValue),DamageEvent,nullptr,nullptr);
	}
}