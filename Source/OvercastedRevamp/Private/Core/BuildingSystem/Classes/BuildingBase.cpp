// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/BuildingSystem/Classes/BuildingBase.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ABuildingBase::ABuildingBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetNetDormancy(DORM_Initial);
	SetNetUpdateFrequency(0.1);

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("Root"));
}

void ABuildingBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass,BlocksLocations);

	DOREPLIFETIME(ThisClass,BlocksRotations);

	DOREPLIFETIME(ThisClass,BlocksTypes);

	DOREPLIFETIME(ThisClass,BlocksLevels);
	
	DOREPLIFETIME(ThisClass,BlocksMeshes);
}

// Called when the game starts or when spawned
void ABuildingBase::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		FTimerHandle ClearGarbageHandle;
		GetWorldTimerManager().SetTimer(ClearGarbageHandle,this,&ThisClass::ClearArraysGarbage,600,true);
	}
}

void ABuildingBase::OnRep_BlocksMeshes()
{
	TArray<int> IndexesToRemove;
	for (int i = 0;i != BlocksMeshes.BlocksMeshes.Num(); i++)
	{
		const EBlockMesh& BlockMesh = BlocksMeshes.BlocksMeshes[i].BlockMesh;
		if (BlockMesh == EBlockMesh::PendingDestroy)
		{
			IndexesToRemove.Add(i);
		}
	}
	
	for (const TPair<EBlockMesh,FIntArray>& Pair : MeshesMap)
	{
		for (const int& Index : Pair.Value.InstanceIndexes)
		{
			HISMComponents.FindRef(Pair.Key)->RemoveInstance(Index);
		}
	}

	for (int i = 0; i != BlocksMeshes.BlocksMeshes.Num(); i++)
	{
		const EBlockMesh& Mesh = BlocksMeshes.BlocksMeshes[i].BlockMesh;
		
        if (!MeshesMap.FindRef(Mesh).ArrayIndexes.Contains(i))
        {
           UBuildingBlockHISM* HISM = HISMComponents.FindRef(Mesh);
	       MeshesMap.FindRef(Mesh).ArrayIndexes.Add(i);
           FTransform Transform = FTransform();
           Transform.SetLocation(BlocksLocations.BlocksLocations[i].BlockLocation);
           Transform.SetRotation(FQuat(0,0,BlocksRotations.BlocksRotations[i].BlockRotation == EBlockRotation::Zero ? 0 : 90,0 ));
           MeshesMap.FindRef(Mesh).InstanceIndexes.Add(HISM->AddInstance(Transform));
        }
	}
} 

void ABuildingBase::SortLayers(TMap<FBuildingLayer, FInts>& MapToSort)
{
	for (int x = -100; x != 100; x++)
	{
		MapToSort.Add(FBuildingLayer(x * 62.5,EBlockType::Wall));
		if (FMath::Modulo(x * 62.5, 125.0) == 0) 
		{
			MapToSort.Add(FBuildingLayer(x * 62.5,EBlockType::Floor));
		}
	}
		
	for (int i = 0; i != BlocksLocations.BlocksLocations.Num(); i++)
	{
		const FBuildingLayer BuildingLayer = FBuildingLayer(BlocksLocations.BlocksLocations[i].BlockLocation.Z,BlocksTypes.BlocksTypes[i].BlockType);
		
		if (MapToSort.Contains(BuildingLayer))
		{
			MapToSort.FindRef(BuildingLayer).Ints.Add(i);
		}
		else
		{
			MapToSort.Add(BuildingLayer).Ints.Add(i);
		}
	}
}

void ABuildingBase::CalculateStability()
{
	SetFoundationsStability();
	
	TMap<FBuildingLayer, FInts> BuildingLayers;

	SortLayers(BuildingLayers);

	for (TPair<FBuildingLayer,FInts>& Value : BuildingLayers.Array())
	{
		for (int x = 0; x != Value.Value.Ints.Num(); x++)
		{
			switch (Value.Key.BlockType)
			{
			case EBlockType::Wall:
				CalculateWallStability(x);
				break;
			case EBlockType::LowWall:
				CalculateWallStability(x);
				break;
			case EBlockType::Floor:
                CalculateFloorStability(x);
				break;
			case EBlockType::Foundation:
				break;
			}
		}
	}
} 

void ABuildingBase::CalculateWallStability(const int& Index)
{
	BlocksStable.MarkItemDirty(BlocksStable.Bytes[Index]);
	uint8& BlockStable = BlocksStable.Bytes[Index].Byte;
	BlockStable = 0;
	
	for (uint8 i = 0; i != 3; i++)
	{
		switch (i)
		{
		case 0:
		    BlockStable += FMath::Clamp(IsWallHaveNeighbours(Index) * 13,0,100);
			break;
		case 1:
		    BlockStable += FMath::Clamp(GetStableUnder(Index) - 49,0,100);
			break;
		default: ;
		}
	}
}

void ABuildingBase::CalculateFloorStability(const int& Index)
{
	const FVector BlockLocation = BlocksLocations.BlocksLocations[Index].BlockLocation;
	
	uint8 FloorStable = 0;
	
	for (uint8 i = 0; i != 4; i++)
	{
		FloorStable += GetFloorSideStable(BlockLocation,i);
	}

	BlocksStable.Bytes[Index].Byte = FloorStable;
}

uint8 ABuildingBase::GetFloorSideStable(const FVector& BlockLocation, const uint8& Side)
{
	FVector Offset1;
	FVector Offset2;
	switch (Side)
	{
	case 0: Offset1 = FVector(0, 300, 0); Offset2 = FVector(0, 150, -125); break;
	case 1: Offset1 = FVector(300, 0, 0); Offset2 = FVector(150, 0, -125); break;
	case 2: Offset1 = FVector(0, -300, 0); Offset2 = FVector(0, -150, -125); break;
	case 3: Offset1 = FVector(-300, 0, 0); Offset2 = FVector(-150, 0, -125); break;
	default: break;
	}
	uint8 LocalHighestStable = 0;
	if (const int FoundIndex = BlocksLocations.BlocksLocations.Find(BlockLocation+Offset1); FoundIndex != INDEX_NONE)
	{
		LocalHighestStable = BlocksStable.Bytes[FoundIndex].Byte;
	}
	
	if (const uint8 FoundIndex = BlocksLocations.BlocksLocations.Find(BlockLocation+Offset2); FoundIndex != INDEX_NONE && BlocksStable.Bytes[FoundIndex].Byte > LocalHighestStable)
	{
		LocalHighestStable = BlocksStable.Bytes[FoundIndex].Byte;
	}
	
	return LocalHighestStable / 4;
}

void ABuildingBase::ClearArraysGarbage()
{
	TArray<int> IndexesToRemove;
	for (int i = 0;i != BlocksMeshes.BlocksMeshes.Num(); i++)
	{
		const EBlockMesh& BlockMesh = BlocksMeshes.BlocksMeshes[i].BlockMesh;
		if (BlockMesh == EBlockMesh::PendingDestroy)
		{
			IndexesToRemove.Add(i);
		}
	}

	for (const int& Index : IndexesToRemove)
	{
		for (TPair<EBlockMesh,FIntArray>& Pair : MeshesMap.Array())
		{
			if (Pair.Value.ArrayIndexes.Contains(Index))
			{
				const int RemoveIndex = Pair.Value.ArrayIndexes.Find(Index);
				Pair.Value.ArrayIndexes.RemoveAt(RemoveIndex);
				Pair.Value.InstanceIndexes.RemoveAt(RemoveIndex);
			}
		}
	}

	for (const int& Index : IndexesToRemove)
	{
		BlocksLocations.MarkArrayDirty();
		BlocksRotations.MarkArrayDirty();
		BlocksTypes.MarkArrayDirty();
		BlocksMeshes.MarkArrayDirty();
		BlocksLevels.MarkArrayDirty();
		BlocksStable.MarkArrayDirty();
		BlocksHealth.MarkArrayDirty();
		
		BlocksLocations.BlocksLocations.RemoveAt(Index);
		BlocksRotations.BlocksRotations.RemoveAt(Index);
		BlocksTypes.BlocksTypes.RemoveAt(Index);
		BlocksMeshes.BlocksMeshes.RemoveAt(Index);
		BlocksLevels.Bytes.RemoveAt(Index);
		BlocksStable.Bytes.RemoveAt(Index);
		BlocksHealth.Floats.RemoveAt(Index);
	}
}

void ABuildingBase::SetFoundationsStability()
{
	for (int i = 0; i != BlocksTypes.BlocksTypes.Num(); i++)
	{
		if (BlocksTypes.BlocksTypes[i].BlockType == EBlockType::Foundation)
		{
			BlocksStable.Bytes[i].Byte = 100;
		}
	}
}

uint8 ABuildingBase::IsWallHaveNeighbours(const int& Index)
{
	TArray<FFastBlockLocation>& BlockLocations = BlocksLocations.BlocksLocations;
	FVector BlockLocation = BlockLocations[Index].BlockLocation;
	EBlockRotation BlockRotation = BlocksRotations.BlocksRotations[Index].BlockRotation;
    uint8 Neighbours = 0;
	
	for (uint8 i = 0; i != 5; i++)
	{
		for (uint8 Layer = 0; Layer != 2; Layer++)
		{
			switch (i)
			{
			case 0:
				if (BlockLocations.Contains(FFastBlockLocation(BlockLocation + (BlockRotation == EBlockRotation::Zero ? FVector(300,0,-62.5*Layer) : FVector (0,300,-62.5*Layer)))))
				{
					Neighbours++;
				}
				break;	
			case 1:
				if (BlockLocations.Contains(FFastBlockLocation(BlockLocation + (BlockRotation == EBlockRotation::Zero ? FVector(150,150,-62.5*Layer) : FVector (150,150,-62.5*Layer)))))
				{
					Neighbours++;
				}
				break;	
			case 2:
				if (BlockLocations.Contains(FFastBlockLocation(BlockLocation + (BlockRotation == EBlockRotation::Zero ? FVector(150,-150,-62.5*Layer) : FVector (-150,150,-62.5*Layer)))))
				{
					Neighbours++;
				}
				break;	
			case 3:
				if (BlockLocations.Contains(FFastBlockLocation(BlockLocation + (BlockRotation == EBlockRotation::Zero ? FVector(-300,0,-62.5*Layer) : FVector (0,-300,-62.5*Layer)))))
				{
					Neighbours++;
				}
				break;	
			case 4:
				if (BlockLocations.Contains(FFastBlockLocation(BlockLocation + (BlockRotation == EBlockRotation::Zero ? FVector(-150,-150,-62.5*Layer) : FVector (-150,-150,-62.5*Layer)))))
				{
					Neighbours++;
				};
				break;
			case 5:
				if (BlockLocations.Contains(FFastBlockLocation(BlockLocation + (BlockRotation == EBlockRotation::Zero ? FVector(-150,150,-62.5*Layer) : FVector (150,-150,-62.5*Layer)))))
				{
					Neighbours++;
				};
				break;
			default:
				break;
			}
		}
	}
	return FMath::Clamp(Neighbours,0,4);
}

uint8 ABuildingBase::GetStableUnder(const int& Index)
{
	TArray<FFastBlockLocation>& BlockLocations = BlocksLocations.BlocksLocations;
	const FVector BlockLocation = BlockLocations[Index].BlockLocation;
	const EBlockType BlockType = BlocksTypes.BlocksTypes[Index].BlockType;
	const EBlockRotation BlockRotation = BlocksRotations.BlocksRotations[Index].BlockRotation;
	
    uint8 HighestStable = 0;
	
	HighestStable += FMath::Clamp(BlockLocations.Find(BlockLocation - FVector(0,0,62.5)),0,100) / 2;
	
	if (HighestStable == 0)
	{
		HighestStable += FMath::Clamp(BlockLocations.Find(BlockLocation - FVector(0,0,125)),0,100) / 2;
	}
	
	for (uint8 i = -1; i != 1; i += 2)
	{
		uint8 LocalHighestStable = 0;
		
		const float XOffset = BlockRotation == EBlockRotation::Zero ? 150 : 0 * i;
		const float YOffset = BlockRotation == EBlockRotation::Zero ? 0 : 150 * i;
		const float ZOffset = BlockType == EBlockType::Wall ? 125 : 62.5;
		
		if (const uint8 LocalStable = BlockLocations.Find(BlockLocation - FVector(XOffset,YOffset,ZOffset)))
		{
			if (LocalStable > LocalHighestStable)
			{
				LocalHighestStable = FMath::Clamp(LocalStable - 13,0,100);
			}
		}

		if (HighestStable < LocalHighestStable)
		{
			HighestStable = LocalHighestStable;
		}
	}

	return HighestStable;
}

void ABuildingBase::AddBlock(const FVector& RelativeLocation, const EBlockRotation& RelativeRotation,
	const EBlockType& BlockType, const EBlockMesh& BlockMesh)
{
	ForceNetUpdate();
	
	const int& Index = BlocksMeshes.BlocksMeshes.Add(FFastBlockMesh(BlockMesh));
	BlocksMeshes.MarkItemDirty(BlocksMeshes.BlocksMeshes[Index]);

	BlocksHealth.Floats.Add(FFastFloat(DT_BuildingBlocks->FindRow<FBuildingBlockParams>(FName(UEnum::GetDisplayValueAsText(BlockMesh).ToString()),"")->MaxHP));
}


