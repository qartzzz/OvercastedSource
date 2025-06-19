// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class OVERCASTEDREVAMP_API DamageTypesData
{
public:
	DamageTypesData();
	~DamageTypesData();
};

UENUM(Blueprintable)
enum class EDamageTypeTags: uint8
{
	CanFarmWood UMETA(DisplayName = "Can farm wood"),
	CanFarmNodes UMETA(DisplayName = "Can farm nodes"),
	CanFarmAnimals UMETA(DisplayName = "Can farm animals"),
};

UENUM(Blueprintable)
enum class EDamageType: uint8
{
	FirearmDamage UMETA(DisplayName = "FirearmDamage"),
	MeleeDamage UMETA(DisplayName = "MeleeDamage"),
	AnimalDamage UMETA(DisplayName = "AnimalDamage"),
};