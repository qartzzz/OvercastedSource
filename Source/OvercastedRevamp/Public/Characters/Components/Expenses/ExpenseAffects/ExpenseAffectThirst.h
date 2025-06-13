// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Components/Expenses/ExpenseAffects/ExpenseAffectBase.h"
#include "ExpenseAffectThirst.generated.h"

/**
 * 
 */
UCLASS()
class OVERCASTEDREVAMP_API UExpenseAffectThirst : public UExpenseAffectBase
{
	GENERATED_BODY()

	virtual void Affect(AActor* TargetActor,const float& ExpenseValue) override;
};
