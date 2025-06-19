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

void AChoppedTreesManager::AddChoppedTree(FChoppedTree ChoppedTree)
{
	FTransform Transform;
	ChoppedTree.ParentComponent->GetInstanceTransform(ChoppedTree.Index,Transform,true);
	Transform.SetLocation(Transform.GetLocation()+FVector(0,0,800000));
	ChoppedTree.ParentComponent->UpdateInstanceTransform(ChoppedTree.Index,Transform,true);
    ChoppedTree.Chopped = true;
	
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
	ChoppedTree.Chopped = false;
}

void AChoppedTreesManager::OnRep_ChoppedTrees()
{
	for(const FChoppedTree& Tree : ChoppedTrees.ChoppedTrees)
	{
		FTransform OutTransform;
		Tree.ParentComponent->GetInstanceTransform(Tree.Index,OutTransform,true);

		if(Tree.Chopped)
		{
			if (OutTransform.GetLocation().Z < 300000)
			{
				OutTransform.SetLocation(OutTransform.GetLocation()+FVector(0,0,800000));
				Tree.ParentComponent->UpdateInstanceTransform(Tree.Index,OutTransform,true);
			}
		}
		else
		{
			if (OutTransform.GetLocation().Z > 300000)
			{
				OutTransform.SetLocation(OutTransform.GetLocation()-FVector(0,0,800000));
				Tree.ParentComponent->UpdateInstanceTransform(Tree.Index,OutTransform,true);
			}
		}
	}
}
