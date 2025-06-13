// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/DamageTypes/DamageTypeBase.h"

float UDamageTypeBase::GetDamageMultiplier(const EObjectTypes& ObjectType)
{
    return DamageMultipliers.Contains(ObjectType) ? DamageMultipliers.FindRef(ObjectType) : 1;
}

