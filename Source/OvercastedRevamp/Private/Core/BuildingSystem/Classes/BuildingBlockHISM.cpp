// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/BuildingSystem/Classes/BuildingBlockHISM.h"

UBuildingBlockHISM::UBuildingBlockHISM()
{
	SetCullDistance(100000);
}

bool UBuildingBlockHISM::RemoveInstanceByData(const FVector& BlockLocation,const EBlockRotation& BlockRotation)
{
	TArray<FTransform> InstancesTransforms;

	const int NumInstances = GetNumInstances();
	
	for (int i = 0; i != NumInstances; i++)
	{
		FTransform OutTransform;
		GetInstanceTransform(i, OutTransform,false);
		GEngine->AddOnScreenDebugMessage(-1,15,FColor::Cyan,OutTransform.GetLocation().ToString());
		InstancesTransforms.Add(OutTransform);
	}

	FTransform InTransform;
	InTransform.SetLocation(BlockLocation);
	
	FQuat InRotator;
	InRotator.Z = BlockRotation == EBlockRotation::Zero ? 0 : 90;
	
	InTransform.SetRotation(InRotator);
	
	const int InstanceID = TransformFind(InstancesTransforms,InTransform);
	
	if (InstanceID != INDEX_NONE)
	{
		RemoveInstance(InstanceID);
		return true;
	}
	return false;
}

void UBuildingBlockHISM::AddInstanceByData(const FVector& BlockLocation, const EBlockRotation& BlockRotation)
{
	FTransform InTransform = FTransform::Identity;
	InTransform.SetLocation(BlockLocation);
	
	FQuat InRotator = FQuat::Identity;
	InRotator.Z = BlockRotation == EBlockRotation::Zero ? 0 : 90;
	
	InTransform.SetRotation(InRotator);

	AddInstance(InTransform);
}

int UBuildingBlockHISM::TransformFind(const TArray<FTransform>& Transforms, const FTransform& Transform) 
{
	for (int i = 0; i != Transforms.Num(); i++)
	{
		if (Transforms[i].GetLocation() == Transform.GetLocation())
		{
			return i;
		}
	}
	GEngine->AddOnScreenDebugMessage(-1,5,FColor::Red,"CantFindTransform");
	return INDEX_NONE;
}


