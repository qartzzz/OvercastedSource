// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Core/BuildingSystem/Structures/BuildingSystemStructures.h"
#include "BuildingBlockHISM.generated.h"

/**
 * 
 */
UCLASS()
class OVERCASTEDREVAMP_API UBuildingBlockHISM : public UHierarchicalInstancedStaticMeshComponent
{
	GENERATED_BODY()
public:
	UBuildingBlockHISM();

	UFUNCTION()
	bool RemoveInstanceByData(const FVector& BlockLocation,const EBlockRotation& BlockRotation);

	void AddInstanceByData(const FVector& BlockLocation,const EBlockRotation& BlockRotation);

	static int TransformFind(const TArray<FTransform>&, const FTransform& Transform);
};

 