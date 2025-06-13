// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Components/Expenses/ExpenseAffects/ExpenseAffectBase.h"
#include "ExpenseAffectPoison.generated.h"

/**
 * 
 */
UCLASS()
class OVERCASTEDREVAMP_API UExpenseAffectPoison : public UExpenseAffectBase
{
	GENERATED_BODY()
	virtual void Affect(AActor* TargetActor,const float& ExpenseValue) override;
};
