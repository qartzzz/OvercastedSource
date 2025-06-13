// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuildingBlockHISM.h"
#include "Core/BuildingSystem/Structures/BuildingSystemStructures.h"
#include "GameFramework/Actor.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "BuildingBase.generated.h"

USTRUCT(BlueprintType)
struct FFastBlockLocation : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY()
	FVector BlockLocation;

    FORCEINLINE bool operator== (const FFastBlockLocation& Other) const
	{
      return BlockLocation == Other.BlockLocation;
	}

	
	FFastBlockLocation(FVector BlockLocation = FVector::ZeroVector) : BlockLocation(BlockLocation) {}
};

USTRUCT(BlueprintType)
struct FFastBlocksLocations : public FFastArraySerializer
{
	GENERATED_BODY()
	
    UPROPERTY()
	TArray<FFastBlockLocation> BlocksLocations;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FastArrayDeltaSerialize<FFastBlockLocation,FFastBlocksLocations>(BlocksLocations, DeltaParams,*this);
	}
	
	FFastBlocksLocations() : BlocksLocations(TArray<FFastBlockLocation>()) {}
};

USTRUCT(BlueprintType)
struct FFastBlockRotation : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY()
	EBlockRotation BlockRotation;

	FFastBlockRotation() : BlockRotation(EBlockRotation::Zero) {}
};

USTRUCT(BlueprintType)
struct FFastBlocksRotations : public FFastArraySerializer
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<FFastBlockRotation> BlocksRotations;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FastArrayDeltaSerialize<FFastBlockRotation,FFastBlocksRotations>(BlocksRotations, DeltaParams,*this);
	}
	
	FFastBlocksRotations() : BlocksRotations(TArray<FFastBlockRotation>()) {}
};

USTRUCT(BlueprintType)
struct FFastBlockType : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY()
	EBlockType BlockType;

	FFastBlockType() : BlockType(EBlockType::Wall) {}
};

USTRUCT(BlueprintType)
struct FFastBlocksTypes : public FFastArraySerializer
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<FFastBlockType> BlocksTypes;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FastArrayDeltaSerialize<FFastBlockType,FFastBlocksTypes>(BlocksTypes, DeltaParams,*this);
	}
	
	FFastBlocksTypes() : BlocksTypes(TArray<FFastBlockType>()) {}
};

USTRUCT(BlueprintType)
struct FFastBlockMesh : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY()
	EBlockMesh BlockMesh;

	FFastBlockMesh(const EBlockMesh InMesh = EBlockMesh::PendingDestroy) : BlockMesh(InMesh) {}
};

USTRUCT(BlueprintType)
struct FFastBlocksMeshes : public FFastArraySerializer
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<FFastBlockMesh> BlocksMeshes;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FastArrayDeltaSerialize<FFastBlockMesh,FFastBlocksMeshes>(BlocksMeshes, DeltaParams,*this);
	}
	
	FFastBlocksMeshes(TArray<EBlockMesh> InBlocksMeshes = {}) : BlocksMeshes(InBlocksMeshes) {}
};

USTRUCT(BlueprintType)
struct FFastByte : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY()
	uint8 Byte;

	FFastByte() : Byte(0) {}
};

USTRUCT(BlueprintType)
struct FFastBytes : public FFastArraySerializer
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<FFastByte> Bytes;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FastArrayDeltaSerialize<FFastByte,FFastBytes>(Bytes, DeltaParams,*this);
	}
	
	FFastBytes() : Bytes(TArray<FFastByte>()) {}
};

USTRUCT(BlueprintType)
struct FFastFloat : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY()
	float Float;

	FFastFloat(const float Float = 0.f) : Float(Float) {}
};
 
USTRUCT(BlueprintType)
struct FFastFloats : public FFastArraySerializer
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<FFastFloat> Floats;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FastArrayDeltaSerialize<FFastFloat,FFastFloats>(Floats, DeltaParams,*this);
	}
	
	FFastFloats() : Floats(TArray<FFastFloat>()) {}
};
struct FInts
{
	TArray<int> Ints;
};
USTRUCT(BlueprintType)
struct FBuildingLayer
{
	GENERATED_BODY()
	
	float Height;
	
	EBlockType BlockType;

	bool operator== (const FBuildingLayer& Other) const
	{
      return BlockType == Other.BlockType && Height == Other.Height;
	}
	
	friend uint32 GetTypeHash(const FBuildingLayer& Other)
	{
		return HashCombine(GetTypeHash(Other.Height), GetTypeHash(Other.BlockType));
	}
	
	FBuildingLayer(float Height = 0,EBlockType BlockType = EBlockType::Wall) : Height(Height), BlockType(BlockType) {}
};

USTRUCT(BlueprintType)
struct FIntArray
{
	GENERATED_BODY()
	UPROPERTY()
	//First int is index in locations array second int is index in HISM
	TArray<int> ArrayIndexes;
    TArray<int> InstanceIndexes;
	FIntArray() : ArrayIndexes({}), InstanceIndexes( {}) {}
};

UCLASS()
class OVERCASTEDREVAMP_API ABuildingBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABuildingBase();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(Replicated,VisibleAnywhere)
	FFastBlocksLocations BlocksLocations;
	
	UPROPERTY(Replicated,VisibleAnywhere)
	FFastBlocksRotations BlocksRotations;
	
	UPROPERTY(Replicated,VisibleAnywhere)
	FFastBlocksTypes BlocksTypes;
	
	UPROPERTY(Replicated,VisibleAnywhere)
	FFastBytes BlocksLevels;
	
	UPROPERTY(ReplicatedUsing="OnRep_BlocksMeshes",VisibleAnywhere)
	FFastBlocksMeshes BlocksMeshes;
	
	UPROPERTY(Replicated,VisibleAnywhere)
	FFastBytes BlocksStable;
	
	UPROPERTY(Replicated,VisibleAnywhere)
	FFastFloats BlocksHealth;

private:
	UFUNCTION()
	void OnRep_BlocksMeshes();

	void SortLayers(TMap<FBuildingLayer,FInts>& MapToSort);
	
	UFUNCTION(CallInEditor)
	void CalculateStability();

	void CalculateWallStability(const int& Index);

	void CalculateFloorStability(const int& Index);

	void SetFoundationsStability();

	uint8 IsWallHaveNeighbours(const int& Index);

	uint8 GetStableUnder(const int& Index);

	uint8 GetFloorSideStable(const FVector& BlockLocation,const uint8& Side);

	void ClearArraysGarbage();
	
    UFUNCTION(BlueprintCallable, Category = "BuildingBase | Blocks")
	void AddBlock(const FVector& RelativeLocation,const EBlockRotation& RelativeRotation,const EBlockType& BlockType,const EBlockMesh& BlockMesh);
	
	UPROPERTY(EditDefaultsOnly, Category = "HISM")
	TMap<EBlockMesh,UBuildingBlockHISM*> HISMComponents;
	
	UPROPERTY(EditDefaultsOnly, Category = "Maps")
	TMap<EBlockMesh,FIntArray> MeshesMap;
	
	UPROPERTY(EditDefaultsOnly, Category = "DataTable")
	UDataTable* DT_BuildingBlocks;

	
};


