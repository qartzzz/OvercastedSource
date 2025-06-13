// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveDataComponent/SaveDataComponent.h"


// Sets default values for this component's properties
USaveDataComponent::USaveDataComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USaveDataComponent::BeginPlay()
{
	Super::BeginPlay();
	if (!GetOwner()->Tags.Contains("Savable"))
	{
		GetOwner()->Tags.Add("Savable");
	}
} 




