// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "GameFramework/Actor.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "ChoppedTreesManager.generated.h"
USTRUCT(BlueprintType)
struct FChoppedTree : public FFastArraySerializerItem
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	int Index;
	UPROPERTY(BlueprintReadWrite)
	bool Chopped;
	UPROPERTY(BlueprintReadWrite)
	UHierarchicalInstancedStaticMeshComponent* ParentComponent;

	bool operator==(const FChoppedTree& Other) const
	{
		return Index == Other.Index && Chopped == Other.Chopped && ParentComponent == Other.ParentComponent;
	}
	
	FChoppedTree(const int InIndex = -1,const bool InChopped = false, UHierarchicalInstancedStaticMeshComponent* InParentComponent = nullptr) : Index(InIndex),Chopped(InChopped), ParentComponent(InParentComponent) {}
};

USTRUCT(BlueprintType)
struct FChoppedTreesArray : public FFastArraySerializer
{
	GENERATED_BODY()
	UPROPERTY()
	TArray<FChoppedTree> ChoppedTrees;
	
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FChoppedTree,FChoppedTreesArray>(ChoppedTrees, DeltaParms,*this);
	}
};

template<>
struct TStructOpsTypeTraits<FChoppedTreesArray> : public TStructOpsTypeTraitsBase2<FChoppedTree>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};

UCLASS()
class OVERCASTEDREVAMP_API AChoppedTreesManager : public AActor
{
	GENERATED_BODY()

public:
	AChoppedTreesManager();

	UFUNCTION(BlueprintCallable,BlueprintAuthorityOnly,Category="ChoppedTreesManager")
    void AddChoppedTree(FChoppedTree ChoppedTree);

	void RespawnTree(FChoppedTree ChoppedTree);
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(ReplicatedUsing="OnRep_ChoppedTrees")
	FChoppedTreesArray ChoppedTrees;

	UFUNCTION()
	void OnRep_ChoppedTrees();
};
