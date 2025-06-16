// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetUserData.h"
#include "BodyPartUserData.generated.h"

enum class EEquipmentBodyPart : uint8;
/**
 * 
 */
UCLASS()
class OVERCASTEDREVAMP_API UBodyPartUserData : public UAssetUserData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	EEquipmentBodyPart BodyPart;
};
