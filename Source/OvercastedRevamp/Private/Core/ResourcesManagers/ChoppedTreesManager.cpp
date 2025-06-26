// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/ResourcesManagers/ChoppedTreesManager.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AChoppedTreesManager::AChoppedTreesManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

void AChoppedTreesManager::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	FDoRepLifetimeParams DoRepLifetimeParams;
	DoRepLifetimeParams.RepNotifyCondition = REPNOTIFY_OnChanged;
	
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass,ChoppedTrees,DoRepLifetimeParams);
}

void AChoppedTreesManager::SpawnTree(FChoppedTree ChoppedTree)
{
	FTransform Transform;
	ChoppedTree.ParentComponent->GetInstanceTransform(ChoppedTree.Index,Transform,true);
	Transform.SetLocation(Transform.GetLocation()+FVector(0,0,800000));
	ChoppedTree.ParentComponent->UpdateInstanceTransform(ChoppedTree.Index,Transform,true);
	
	ChoppedTrees.ChoppedTrees.Add(ChoppedTree);
	ChoppedTrees.MarkItemDirty(ChoppedTrees.ChoppedTrees.Last());
}

void AChoppedTreesManager::RespawnTree(FChoppedTree ChoppedTree)
{
	ChoppedTrees.MarkItemDirty(ChoppedTrees.ChoppedTrees[ChoppedTrees.ChoppedTrees.Find(ChoppedTree)]);
	
	FTransform Transform;
	ChoppedTree.ParentComponent->GetInstanceTransform(ChoppedTree.Index,Transform,true);
	Transform.SetLocation(Transform.GetLocation()-FVector(0,0,800000));
	ChoppedTree.ParentComponent->UpdateInstanceTransform(ChoppedTree.Index,Transform,true);
}

void AChoppedTreesManager::OnRep_ChoppedTrees()
{
	for (const FChoppedTree& ChoppedTree : LocalChoppedTrees)
	{
		const FChoppedTree LocalChoppedTree = ChoppedTree;
		
		if (!ChoppedTrees.ChoppedTrees.Contains(ChoppedTree))
		{
			FTransform Transform;
			ChoppedTree.ParentComponent->GetInstanceTransform(ChoppedTree.Index,Transform,true);
			if (Transform.GetLocation().Z > 500000)
			{
				Transform.SetLocation(Transform.GetLocation()-FVector(0,0,800000));
				ChoppedTree.ParentComponent->UpdateInstanceTransform(ChoppedTree.Index,Transform,true);
			}
		}
		else
		{
			FTransform OutTransform;
			ChoppedTree.ParentComponent->GetInstanceTransform(ChoppedTree.Index,OutTransform,true);

			if (OutTransform.GetLocation().Z < 300000)
			{
				OutTransform.SetLocation(OutTransform.GetLocation()+FVector(0,0,800000));
				ChoppedTree.ParentComponent->UpdateInstanceTransform(ChoppedTree.Index,OutTransform,true);
			}
		}
	}
	
	LocalChoppedTrees = ChoppedTrees.ChoppedTrees;
}

void AChoppedTreesManager::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		FTimerHandle RespawnTimerHandle;
		GetWorldTimerManager().SetTimer(RespawnTimerHandle,this,&ThisClass::RespawnTrees,RespawnRate,true,FMath::RandRange(RespawnRate * -0.97,1.0));
	}
}

void AChoppedTreesManager::RespawnTrees()
{
	bool Respawned = false;
	
	for (const FChoppedTree& Tree : ChoppedTrees.ChoppedTrees)
	{
		if (FMath::RandRange(0,100) > 66)
		{
			Respawned = true;
			RespawnTree(Tree);
		}
	}

	if (Respawned)
	{
		ChoppedTrees.MarkArrayDirty();
	}
}