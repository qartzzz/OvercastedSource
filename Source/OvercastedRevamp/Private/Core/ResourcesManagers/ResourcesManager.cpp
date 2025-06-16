// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/ResourcesManagers/ResourcesManager.h"

#include "Core/DamageTypes/DamageTypeBase.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/GameModeBase.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AResourcesManager::AResourcesManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	bReplicates = true;
	SetNetUpdateFrequency(1);
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	
	StoneNodes = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>("StoneNodes");
	StoneNodes->SetupAttachment(RootComponent);
	SulfurNodes = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>("SulfurNodes");
	SulfurNodes->SetupAttachment(RootComponent);
	MetalNodes = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>("MetalNodes");
	MetalNodes->SetupAttachment(RootComponent);
	Stumps = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>("Stumps");
	Stumps->SetupAttachment(RootComponent);
	LogPiles = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>("LogPiles");
	LogPiles->SetupAttachment(RootComponent);
	SmallStoneNodes = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>("SmallStoneNodes");
	SmallStoneNodes->SetupAttachment(RootComponent);
	SmallMetalNodes = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>("SmallMetalNodes");
	SmallMetalNodes->SetupAttachment(RootComponent);
	SmallSulfurNodes = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>("SmallSulfurNodes");
	SmallSulfurNodes->SetupAttachment(RootComponent);
	Barrels = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>("Barrels");
	Barrels->SetupAttachment(RootComponent);
	PoorBarrels = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>("PoorBarrels");
	PoorBarrels->SetupAttachment(RootComponent);
	OilBarrels = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>("OilBarrels");
	OilBarrels->SetupAttachment(RootComponent);

	DT_ResourcesData = ConstructorHelpers::FObjectFinder<UDataTable>(TEXT("/Game/Core/ResourcesManager/DT_ResourcesProperties.DT_ResourcesProperties")).Object;
}

void AResourcesManager::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass,AvailableResources);
}

// Called when the game starts or when spawned
void AResourcesManager::BeginPlay()
{
	Super::BeginPlay();
	HISMs.Add(static_cast<EResourceType>(1),StoneNodes);
	HISMs.Add(static_cast<EResourceType>(2),MetalNodes);
	HISMs.Add(static_cast<EResourceType>(3),SulfurNodes);
	HISMs.Add(static_cast<EResourceType>(4),SmallStoneNodes);
	HISMs.Add(static_cast<EResourceType>(5),SmallMetalNodes);
	HISMs.Add(static_cast<EResourceType>(6),SmallSulfurNodes);
	HISMs.Add(static_cast<EResourceType>(7),Stumps);
	HISMs.Add(static_cast<EResourceType>(8),LogPiles);

	if (HasAuthority())
	{
		FTimerHandle TimerHandleResourcesSpawn;
		GetWorldTimerManager().SetTimer(TimerHandleResourcesSpawn,this,&AResourcesManager::SpawnBunchOfAllResources,ResourcesRespawnRate,true,ResourcesRespawnRate * FMath::RandRange(-0.9,2.0));
	}
		
}

FVector AResourcesManager::CalculateTraceLocation() const
{
	return GetActorLocation() + FVector(FMath::RandRange(-15000.0,15000.0), FMath::RandRange(-15000.0,15000.0), 30000);
}

bool AResourcesManager::SpawnBunchOfResources(const EResourceType ResourceType,const int32 Multiplier = 1)
{
	FResourceData* ResourceData = DT_ResourcesData->FindRow<FResourceData>(FName(UEnum::GetValueAsString(ResourceType).RightChop(15)),"");
	const int PlayerBasedResourceAmount = FMath::Clamp(GetWorld()->GetAuthGameMode()->GetNumPlayers(),20,50);
	const int ClampedResourceAmount = FMath::Clamp(ResourcesInGrid.FindRef(ResourceType) + PlayerBasedResourceAmount * Multiplier,0,ResourceData->MaxAmountInGrid);
	
	for (int i = 0; i != ClampedResourceAmount; i++)
	{
		FHitResult Hit; 
		constexpr ECollisionChannel CollisionChannel = ECC_WorldStatic;
		FVector Start = CalculateTraceLocation();
		GetWorld()->LineTraceSingleByChannel(Hit,Start,Start - FVector(0,0,50000),CollisionChannel);
		
		if (ResourcesInGrid.FindRef(ResourceType) < ResourceData->MaxAmountInGrid && Hit.IsValidBlockingHit() && Hit.GetActor())
		{
			if (Hit.Normal.Z > ResourceData->MinResourceNormal.Normal && Hit.Normal.Z < ResourceData->MaxResourceNormal.Normal)
			{
				const float AngleChance = FMath::GetMappedRangeValueClamped(FVector2D(ResourceData->MinResourceNormal.Normal,ResourceData->MaxResourceNormal.Normal),FVector2D(ResourceData->MinResourceNormal.Chance,ResourceData->MaxResourceNormal.Chance),Hit.Normal.Z);
				const float PhysMatChance = ResourceData->FindSpawnChance(Hit.PhysMaterial.Get());
				const float SpawnChance = (PhysMatChance + AngleChance) / 2;
				
				if (FMath::RandRange(0,100) < SpawnChance)
				{
					AddResourceToArray(ResourceType,Hit.Location,Hit.Normal.Rotation()-FRotator(90,0,0));
				}
			}
		}
	}
	return false;
}

void AResourcesManager::SpawnBunchOfAllResources()
{
	ForceNetUpdate();
	if (HasAuthority())
	{
		for (int i = 1; i != static_cast<int>(EResourceType::Max) - 1; i++)
		{
			SpawnBunchOfResources(static_cast<EResourceType>(i),1);
		}
	}
}

void AResourcesManager::OnRep_Resources()
{
	for (const FFastResource& Resource : AvailableResources.Entries)
	{
		UHierarchicalInstancedStaticMeshComponent* HISM = HISMs.FindRef(Resource.Type);
		if (HISM->IsValidInstance(Resource.HISMIndex))
		{
			FTransform OutInstanceTransform;
			HISM->GetInstanceTransform(Resource.HISMIndex,OutInstanceTransform);
			if (OutInstanceTransform.GetLocation() != Resource.Location)
			{
				GEngine->AddOnScreenDebugMessage(-1,5,FColor::Red,"ResourcesLocationsNotIdentical");
				HISM->UpdateInstanceTransform(Resource.HISMIndex,FTransform(Resource.Rotation,Resource.Location),true);
			}
		}
		else
		{
			HISM->AddInstance(FTransform(Resource.Rotation,Resource.Location),true);
		}
	}
}

void AResourcesManager::AddResourceToArray(const EResourceType& ResourceType,const FVector& Location,const FRotator& Rotation)
{
	if (!ResourcesInGrid.Find(ResourceType))
	{
		ResourcesInGrid.Add(ResourceType,1);
	}
	else
	{
		ResourcesInGrid.Add(ResourceType,ResourcesInGrid.FindRef(ResourceType) + 1);
	}
	AvailableResources.Entries.Add(FFastResource(ResourceType,Location,Rotation,HISMs.FindRef(ResourceType)->AddInstance(FTransform(Rotation,Location,FVector(1,1,1)),true)));
	AvailableResources.MarkItemDirty(AvailableResources.Entries.Last());
}

void AResourcesManager::RemoveResourceFromArray(FFastResource& Resource)
{
	ForceNetUpdate();
	if (!ResourcesInGrid.Find(Resource.Type))
	{
		ResourcesInGrid.Add(Resource.Type,0);
	}
	else
	{
		ResourcesInGrid.Add(Resource.Type,ResourcesInGrid.FindRef(Resource.Type) - 1);
	}
	
	UHierarchicalInstancedStaticMeshComponent* HISM = HISMs.FindRef(Resource.Type);

	HISM->UpdateInstanceTransform(Resource.HISMIndex,FTransform::Identity,true);
	Resource.Location = FVector::ZeroVector;
	
	AvailableResources.MarkItemDirty(Resource);
}

float AResourcesManager::InternalTakeRadialDamage(float Damage, struct FRadialDamageEvent const& RadialDamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
   const FHitResult HitResult = RadialDamageEvent.ComponentHits[0];
	
	typedef UHierarchicalInstancedStaticMeshComponent UHISM;
	if (HitResult.Component.IsValid() && RadialDamageEvent.DamageTypeClass.Get() == UDamageTypeBase::StaticClass() && IIDamageTypeInfo::Execute_HaveDamageTypeTag(RadialDamageEvent.DamageTypeClass->GetDefaultObject<UDamageTypeBase>(),EDamageTypeTags::CanFarmNodes))
	{
		const FName& ComponentTag = HitResult.Component->ComponentTags[0];
		if (DT_ResourcesData->GetRowNames().Contains(ComponentTag))
		{
			const FResourceData* ResourceData = DT_ResourcesData->FindRow<FResourceData>(ComponentTag,"");
			if (!ResourceData->PickUpReward.IsEmpty())
			{
				const UHISM* HISM = Cast<UHISM>(HitResult.Component);
				
				FTransform OutInstanceTransform;
				HISM->GetInstanceTransform(HitResult.Item,OutInstanceTransform,true);

				GetWorld()->SpawnActor<AActor>(ResourceData->ResourceClass,OutInstanceTransform);
				
				const FVector ResourceLocation = OutInstanceTransform.GetLocation();
				const FRotator ResourceRotation = OutInstanceTransform.GetRotation().Rotator();
    		
				const EResourceType ResourceType = GetKeyByHISM(Cast<UHISM>(HitResult.Component.Get()));
				const FFastResource ResourceToRemove = FFastResource(ResourceType,ResourceLocation,ResourceRotation,HitResult.Item);
    		
				RemoveResourceFromArray(AvailableResources.Entries[AvailableResources.Entries.Find(ResourceToRemove)]);
				return true;
			}
		}
	}
	return false;
}

bool AResourcesManager::Interact_Implementation(APlayerController* PlayerController, const FHitResult& HitResult)
{
	typedef UHierarchicalInstancedStaticMeshComponent UHISM;
    if (HitResult.Component.IsValid())
    {
    	const FName& ComponentTag = HitResult.Component->ComponentTags[0];
    	if (DT_ResourcesData->GetRowNames().Contains(ComponentTag))
    	{
    		const FResourceData* ResourceData = DT_ResourcesData->FindRow<FResourceData>(ComponentTag,"");
    		if (!ResourceData->PickUpReward.IsEmpty())
    		{
    			const UHISM* HISM = Cast<UHISM>(HitResult.Component);
    			FTransform OutInstanceTransform;
    			HISM->GetInstanceTransform(HitResult.Item,OutInstanceTransform,true);
    		
    			const FVector ResourceLocation = OutInstanceTransform.GetLocation();
    			const FRotator ResourceRotation = OutInstanceTransform.GetRotation().Rotator();
    		
    			const EResourceType ResourceType = GetKeyByHISM(Cast<UHISM>(HitResult.Component.Get()));
    			const FFastResource ResourceToRemove = FFastResource(ResourceType,ResourceLocation,ResourceRotation,HitResult.Item);
    		
    			RemoveResourceFromArray(AvailableResources.Entries[AvailableResources.Entries.Find(ResourceToRemove)]);

    			UActorComponent* ActorComponent = PlayerController->GetPawn()->GetComponentsByTag(UInventoryComponent::StaticClass(),"Inventory")[0];
    			UInventoryComponent* InventoryComponent = Cast<UInventoryComponent>(ActorComponent);
    			InventoryComponent->AddItems(ResourceData->PickUpReward);
    			return true;
    		}
    	}
    }
	return false;
}


EResourceType AResourcesManager::GetKeyByHISM(const UHierarchicalInstancedStaticMeshComponent* HISM)
{
	for (const TPair<EResourceType,UHierarchicalInstancedStaticMeshComponent*> Pair : HISMs.Array())
	{
		if (Pair.Value == HISM)
		{
			return Pair.Key;
		}
	}
	return EResourceType::None;
}