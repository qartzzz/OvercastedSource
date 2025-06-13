// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Components/Expenses/ExpenseAffects/ExpenseAffectBase.h"
void UExpenseAffectBase::Affect(AActor* TargetActor,const float& ExpenseValue)
{
	if (ExpenseValue < 100)
	{
		const FDamageEvent DamageEvent;
		TargetActor->TakeDamage(FMath::GetMappedRangeValueClamped(FVector2D(0,100),FVector2D(0.2,0),ExpenseValue),DamageEvent,nullptr,nullptr);
	}
}
 