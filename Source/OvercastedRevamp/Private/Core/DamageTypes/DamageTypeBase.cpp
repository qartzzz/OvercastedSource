// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/DamageTypes/DamageTypeBase.h"

float UDamageTypeBase::GetDamageMultiplier(const EObjectTypes& ObjectType)
{
    return DamageMultipliers.Contains(ObjectType) ? DamageMultipliers.FindRef(ObjectType) : 1;
}

bool UDamageTypeBase::HaveDamageTypeTag_Implementation(const EDamageTypeTags Tag)
{
    return Tags.Contains(Tag);
}

const TArray<EDamageTypeTags> UDamageTypeBase::GetDamageTypeTags_Implementation()
{
    return Tags;
}

const EDamageType UDamageTypeBase::GetDamageType_Implementation()
{
    return DamageType;
}

