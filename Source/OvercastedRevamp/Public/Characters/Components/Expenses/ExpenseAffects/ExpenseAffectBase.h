// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Engine/DamageEvents.h"
#include "CoreMinimal.h"
#include "ExpenseAffectBase.generated.h"

/**
 * 
 */
UCLASS()
class OVERCASTEDREVAMP_API UExpenseAffectBase : public UObject
{
	GENERATED_BODY()

public:
	virtual void Affect(AActor* TargetActor,const float& ExpenseValue);
};
