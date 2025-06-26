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

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);

	WoodenWall = CreateDefaultSubobject<UBuildingBlockHISM>("WoodenWall");
	WoodenWall->SetupAttachment(Root);
	WoodenLowWall = CreateDefaultSubobject<UBuildingBlockHISM>("WoodenLowWall");
	WoodenLowWall->SetupAttachment(Root);
	WoodenDoorway = CreateDefaultSubobject<UBuildingBlockHISM>("WoodenDoorway");
	WoodenDoorway->SetupAttachment(Root);
	WoodenWindow = CreateDefaultSubobject<UBuildingBlockHISM>("WoodenWindow");
	WoodenWindow->SetupAttachment(Root);
	WoodenFloor = CreateDefaultSubobject<UBuildingBlockHISM>("WoodenFloor");
	WoodenFloor->SetupAttachment(Root);
	WoodenFoundation = CreateDefaultSubobject<UBuildingBlockHISM>("WoodenFoundation");
	WoodenFoundation->SetupAttachment(Root);
	DestroyedBlocks = CreateDefaultSubobject<UBuildingBlockHISM>("DestroyedBlocks");
	DestroyedBlocks->SetupAttachment(Root);

	DT_BuildingBlocks = ConstructorHelpers::FObjectFinder<UDataTable>(TEXT("/Game/Core/BuildingSystem/StructureBasedBuildingSystem/DataTable/DT_BuildingBlocks.DT_BuildingBlocks")).Object;
}

void ABuildingBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass,BlocksLocations);

	DOREPLIFETIME(ThisClass,BlocksRotations);

	DOREPLIFETIME(ThisClass,BlocksTypes);

	DOREPLIFETIME(ThisClass,BlocksLevels);

	DOREPLIFETIME(ThisClass,BlocksStable);
	
	DOREPLIFETIME(ThisClass,BlocksMeshes);

	DOREPLIFETIME(ThisClass,BlocksHealth);
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

	HISMComponents.Add(EBlockMesh::WoodenWall,WoodenWall);
	HISMComponents.Add(EBlockMesh::WoodenLowWall,WoodenLowWall);
	HISMComponents.Add(EBlockMesh::WoodenWindow,WoodenWindow);
	HISMComponents.Add(EBlockMesh::WoodenDoorway,WoodenDoorway);
	HISMComponents.Add(EBlockMesh::WoodenFloor,WoodenFloor);
	HISMComponents.Add(EBlockMesh::WoodenFoundation,WoodenFoundation);
	HISMComponents.Add(EBlockMesh::None,DestroyedBlocks);
}

void ABuildingBase::OnRep_BlocksMeshes()
{
	for (int i = 0; i != BlocksLocations.BlocksLocations.Num(); i++)
	{
		const int LocalIndex = LocalBlockLocations.Find(BlocksLocations.BlocksLocations[i]);
		
		if (LocalIndex == INDEX_NONE)
		{
			HISMComponents.FindRef(BlocksMeshes.BlocksMeshes[i].BlockMesh)->AddInstanceByData(BlocksLocations.BlocksLocations[i].BlockLocation,BlocksRotations.BlocksRotations[i].BlockRotation);
		}
		else
		{
			if (LocalBlockMeshes[LocalIndex].BlockMesh != BlocksMeshes.BlocksMeshes[i].BlockMesh)
			{
				HISMComponents.FindRef(BlocksMeshes.BlocksMeshes[i].BlockMesh)->AddInstanceByData(BlocksLocations.BlocksLocations[i].BlockLocation,BlocksRotations.BlocksRotations[i].BlockRotation);
				HISMComponents.FindRef(LocalBlockMeshes[LocalIndex].BlockMesh)->RemoveInstanceByData(LocalBlockLocations[LocalIndex].BlockLocation,LocalBlockRotations[LocalIndex].BlockRotation);
			}
		}
	}

	for (int i = 0; i != LocalBlockLocations.Num(); i++)
	{
		if (!BlocksLocations.BlocksLocations.Contains(LocalBlockLocations[i]))
		{
			HISMComponents.FindRef(LocalBlockMeshes[i].BlockMesh)->RemoveInstanceByData(LocalBlockLocations[i].BlockLocation,LocalBlockRotations[i].BlockRotation);
		}
	}
	
	LocalBlockMeshes = BlocksMeshes.BlocksMeshes;

	LocalBlockLocations = BlocksLocations.BlocksLocations;

	LocalBlockRotations = BlocksRotations.BlocksRotations;
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
	BlocksLocations.MarkArrayDirty();
	BlocksRotations.MarkArrayDirty();
	BlocksTypes.MarkArrayDirty();
	BlocksMeshes.MarkArrayDirty();
	BlocksLevels.MarkArrayDirty();
	BlocksStable.MarkArrayDirty();
	BlocksHealth.MarkArrayDirty();
	
	TArray<int> IndexesToRemove;
	for (int i = 0;i != BlocksLocations.BlocksLocations.Num(); i++)
	{
		if (BlocksLocations.BlocksLocations[i].BlockLocation.Z > 1000000)
		{
			IndexesToRemove.Add(i);
		}
	}
 
	for (const int& Index : IndexesToRemove)
	{
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

void ABuildingBase::AddBlock(const FVector RelativeLocation, const EBlockRotation RelativeRotation,
	const EBlockType BlockType, const EBlockMesh BlockMesh)
{
	ForceNetUpdate();
	BlocksLocations.BlocksLocations.Add(RelativeLocation);
	BlocksLocations.MarkItemDirty(BlocksLocations.BlocksLocations.Last());

	BlocksRotations.BlocksRotations.Add(RelativeRotation);
	BlocksRotations.MarkItemDirty(BlocksRotations.BlocksRotations.Last());
	
	BlocksMeshes.BlocksMeshes.Add(FFastBlockMesh(BlockMesh));
	BlocksMeshes.MarkItemDirty(BlocksMeshes.BlocksMeshes.Last());
	
	BlocksHealth.Floats.Add(FFastFloat(DT_BuildingBlocks->FindRow<FBuildingBlockParams>(FName(UEnum::GetValueAsString(BlockMesh).RightChop(12)),"")->MaxHP));
	BlocksHealth.MarkItemDirty(BlocksHealth.Floats.Last());

	HISMComponents.FindRef(BlockMesh)->AddInstanceByData(RelativeLocation, RelativeRotation);
}

void ABuildingBase::RemoveBlock(const FVector RelativeLocation, const EBlockRotation RelativeRotation, const EBlockMesh BlockMesh)
{
	ForceNetUpdate();
	const int IndexToRemove = BlocksLocations.BlocksLocations.Find(RelativeLocation);

	if (IndexToRemove != INDEX_NONE)
	{
		BlocksLocations.BlocksLocations[IndexToRemove].BlockLocation += FVector(0,0,1000000);
		BlocksLocations.MarkItemDirty(BlocksLocations.BlocksLocations[IndexToRemove]);
		
		BlocksMeshes.BlocksMeshes[IndexToRemove].BlockMesh = EBlockMesh::None;
		BlocksMeshes.MarkItemDirty(BlocksMeshes.BlocksMeshes[IndexToRemove]);
		HISMComponents.FindRef(BlockMesh)->RemoveInstanceByData(RelativeLocation,RelativeRotation);
	}
}


