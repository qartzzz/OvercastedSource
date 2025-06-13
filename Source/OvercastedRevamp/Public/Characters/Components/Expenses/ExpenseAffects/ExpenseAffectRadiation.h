// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Components/Expenses/ExpenseAffects/ExpenseAffectBase.h"
#include "ExpenseAffectRadiation.generated.h"

/**
 * 
 */
UCLASS()
class OVERCASTEDREVAMP_API UExpenseAffectRadiation : public UExpenseAffectBase
{
	GENERATED_BODY()
public:
	UExpenseAffectRadiation();
	
	virtual void Affect(AActor* TargetActor,const float& ExpenseValue) override;

	UPROPERTY(EditAnywhere)
	USoundBase* RadiationSound;
};
