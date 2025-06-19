// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Components/Inventory/InventoryComponent.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Core/DamageTypes/DamageTypesData.h"
#include "GameFramework/Actor.h"
#include "Interfaces/IPlayerWorldInteraction.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "ResourcesManager.generated.h"

UENUM()
enum class EResourceType : uint8
{
	None,
	StoneNode,
	MetalOreNode, 
	SulfurOreNode,
	SmallStoneNode,
	SmallMetalOreNode,
	SmallSulfurOreNode,
	Stump,
	LogPile,
	OilBarrel,
	PoorBarrel,
	Barrel,
	Hemp,
	Mushroom,
	Max
};

USTRUCT()
struct FFastResource : public FFastArraySerializerItem
{
    GENERATED_BODY()

	UPROPERTY()
	EResourceType Type;
	UPROPERTY()
	FVector Location;
	UPROPERTY()
	FRotator Rotation;
	UPROPERTY()
	int HISMIndex;

	FORCEINLINE bool operator==(const FFastResource& Other) const
	{
		return Type == Other.Type && FVector::PointsAreNear(Location,Other.Location,1) && Rotation.Equals(Other.Rotation,1) && HISMIndex == Other.HISMIndex;
	}
	FFastResource(const EResourceType& InType = EResourceType::None,const FVector& InLocation = FVector::ZeroVector,const FRotator& InRotation = FRotator::ZeroRotator,const int& InHISMIndex = 0) : Type(InType), Location(InLocation), Rotation(InRotation),HISMIndex(InHISMIndex) {}
};

USTRUCT()
struct FFastResources : public FFastArraySerializer
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<FFastResource> Entries;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FFastResource,FFastResources>(Entries, DeltaParms,*this);
	}
};

template<>
struct TStructOpsTypeTraits<FFastResources> : public TStructOpsTypeTraitsBase2<FFastResources>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};

USTRUCT()
struct FResourceSpawnAngleSettings
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	float Normal = 0;
	
	UPROPERTY(EditDefaultsOnly)
	float Chance = 0;
	
	FResourceSpawnAngleSettings(const float InNormal = 0,const float InChance = 0) : Normal(InNormal),Chance(InChance) {}
};
USTRUCT()
struct FResourcePhysMaterialSpawnChance
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UPhysicalMaterial> PhysicalMaterial;
	
	UPROPERTY(EditAnywhere)
	float SpawnChance;
	
	FResourcePhysMaterialSpawnChance () : PhysicalMaterial(nullptr), SpawnChance(0) {}
};
USTRUCT(BlueprintType)
struct FResourceData : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere,Category="ActorSpawn")
	TSubclassOf<AActor> ResourceClass;

	UPROPERTY(EditAnywhere,Category="ActorSpawn")
	EDamageTypeTags DamageTagForTrigger;
	
	UPROPERTY(EditAnywhere)
	//Chance in range 0 - 100
	TArray<FResourcePhysMaterialSpawnChance> PhysicalMaterialChances;
	
	//Z normal in range 0 - 1 , spawn change in range 0 - 100
	UPROPERTY(EditAnywhere)
	FResourceSpawnAngleSettings MinResourceNormal;
	
	//Z normal in range 0 - 1 , spawn change in range 0 - 100
	UPROPERTY(EditAnywhere)
	FResourceSpawnAngleSettings MaxResourceNormal;
	
	UPROPERTY(EditAnywhere)
	int MaxAmountInGrid;

	UPROPERTY(EditAnywhere,Category = "Pickup")
	TArray<FInventorySlot> PickUpReward;

	UPROPERTY(EditAnywhere,Category = "Pickup")
	TObjectPtr<USoundBase> PickUpSound;
	
	float FindSpawnChance(UPhysicalMaterial* PhysicalMaterial)
	{
		for (const FResourcePhysMaterialSpawnChance& Chance : PhysicalMaterialChances)
		{
			if (Chance.PhysicalMaterial == PhysicalMaterial)
			{
				return Chance.SpawnChance;
			}
		}
		return 0;
	}
};

UCLASS()
class OVERCASTEDREVAMP_API AResourcesManager : public AActor , public IIPlayerWorldInteraction
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AResourcesManager();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere)
	float ResourcesRespawnRate = 300;
	
private:
	
	virtual float InternalTakePointDamage(float Damage, struct FPointDamageEvent const& PointDamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
	bool Interact_Implementation(APlayerController* PlayerController,const FHitResult& HitResult) override;
	
	UFUNCTION()
	bool SpawnBunchOfResources(const EResourceType ResourceType,const int32 Multiplier);

	void SpawnBunchOfAllResources();
	
	FVector CalculateTraceLocation() const;
	
	TMap<EResourceType,int> ResourcesInGrid;

	EResourceType GetKeyByHISM(const UHierarchicalInstancedStaticMeshComponent* HISM);
	
	UPROPERTY()
	UDataTable* DT_ResourcesData;

	UPROPERTY(ReplicatedUsing=OnRep_Resources)
	FFastResources AvailableResources;

	UFUNCTION()
	void OnRep_Resources();

	void AddResourceToArray(const EResourceType& ResourceType,const FVector& Location,const FRotator& Rotation);

	void RemoveResourceFromArray(FFastResource& Resource);

	UFUNCTION(NetMulticast,Unreliable)
	void MC_SpawnPickUpSound(const FName ResourceName,const FVector Location);
	
	UPROPERTY()
	TMap<EResourceType,UHierarchicalInstancedStaticMeshComponent*> HISMs;
	
	UPROPERTY(VisibleAnywhere,meta = (AllowPrivateAccess=true))
	UHierarchicalInstancedStaticMeshComponent* StoneNodes;

	UPROPERTY(VisibleAnywhere,meta = (AllowPrivateAccess=true))
	UHierarchicalInstancedStaticMeshComponent* SulfurNodes;
	
	UPROPERTY(VisibleAnywhere,meta = (AllowPrivateAccess=true))
	UHierarchicalInstancedStaticMeshComponent* MetalNodes;
	
	UPROPERTY(VisibleAnywhere,meta = (AllowPrivateAccess=true))
	UHierarchicalInstancedStaticMeshComponent* Stumps;

	UPROPERTY(VisibleAnywhere,meta = (AllowPrivateAccess=true))
	UHierarchicalInstancedStaticMeshComponent* LogPiles;
	
	UPROPERTY(VisibleAnywhere,meta = (AllowPrivateAccess=true))
	UHierarchicalInstancedStaticMeshComponent* SmallStoneNodes;
	
	UPROPERTY(VisibleAnywhere,meta = (AllowPrivateAccess=true))
	UHierarchicalInstancedStaticMeshComponent* SmallMetalNodes;
	
	UPROPERTY(VisibleAnywhere,meta = (AllowPrivateAccess=true))
	UHierarchicalInstancedStaticMeshComponent* SmallSulfurNodes;

	UPROPERTY(VisibleAnywhere,meta = (AllowPrivateAccess=true))
	UHierarchicalInstancedStaticMeshComponent* Barrels;

	UPROPERTY(VisibleAnywhere,meta = (AllowPrivateAccess=true))
	UHierarchicalInstancedStaticMeshComponent* PoorBarrels;

	UPROPERTY(VisibleAnywhere,meta = (AllowPrivateAccess=true))
	UHierarchicalInstancedStaticMeshComponent* OilBarrels;
	
	UPROPERTY(VisibleAnywhere,meta = (AllowPrivateAccess=true))
	UHierarchicalInstancedStaticMeshComponent* HempBushes;

	UPROPERTY(VisibleAnywhere,meta = (AllowPrivateAccess=true))
	UHierarchicalInstancedStaticMeshComponent* Mushrooms;
};


