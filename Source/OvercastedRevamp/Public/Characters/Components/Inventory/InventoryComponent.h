// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryDataTypes.h"
#include "Core/CraftingSystem/CraftSystemStructures.h"
#include "Components/ActorComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "InventoryComponent.generated.h"

UENUM(BlueprintType,Blueprintable)
enum class EInventoryFeatures : uint8
{
	None UMETA(DisplayName = "None"),
	UsingFilters UMETA(DisplayName = "UsingFilters"),
	ForEquipment UMETA(DisplayName = "ForEquipment"),
};
USTRUCT(BlueprintType,Blueprintable)
struct FSlotItem
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Amount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemTags ItemTags;
	
	FSlotItem(const FName InName = "",const int InAmount = 0,TArray<FItemTags> InItemTags = {}) : ItemID(InName),Amount(InAmount),ItemTags({}) {}
};

//struct FContentArray;
USTRUCT(BlueprintType)
struct FInventorySlot
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "ItemID", MakeStructureDefaultValue = ""))
	FName ItemID;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Amount", MakeStructureDefaultValue = "0"))
	int Amount;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FItemTags> ItemTags;
	
	bool operator==(const FInventorySlot& Data) const
	{
		return Data.ItemID == ItemID && Data.Amount == Amount && Data.ItemTags == ItemTags;
	}

	bool operator!=(const FInventorySlot& Data) const
	{
		return Data.ItemID != ItemID || Data.Amount != Amount || Data.ItemTags != ItemTags;
	}
	FInventorySlot(FName InItemID = "",int InAmount = 0,TArray<FItemTags> InItemTags = {}) : ItemID(""),Amount(0),ItemTags({}){}
};

USTRUCT(BlueprintType,Blueprintable)
struct FSerializedInventorySlot : public FFastArraySerializerItem
{
	GENERATED_BODY()
	 
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FInventorySlot Slot;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FInventorySlot> SlotContent;

	UPROPERTY(BlueprintReadWrite)
	uint8 UniqueSlotID;
	
	bool operator!=(const FSerializedInventorySlot& Data) const
	{
		return Data.Slot != Slot || Data.SlotContent != SlotContent;
	}
	
	FSerializedInventorySlot(const FInventorySlot& InSlot = FInventorySlot(),const TArray<FInventorySlot>& InSlotContent = {}) : Slot(InSlot),SlotContent(InSlotContent),UniqueSlotID(-1) {}
};

USTRUCT()
struct FContentArray: public FFastArraySerializer
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<FSerializedInventorySlot> Items;
	
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FSerializedInventorySlot,FContentArray>(Items, DeltaParms,*this);
	}
};

template<>
struct TStructOpsTypeTraits<FContentArray> : public TStructOpsTypeTraitsBase2<FContentArray>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class OVERCASTEDREVAMP_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
UInventoryComponent();



/**DELEGATES */

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemsChanged,const TArray<int>&,ChangedIndexes);
	UPROPERTY(BlueprintAssignable)
	FOnItemsChanged OnItemsChanged;

	
/**PROPERTIES*/

	
	UPROPERTY(BlueprintReadOnly,EditAnywhere,Category = "Inventory")
	int InventorySize = 30;
	
	UPROPERTY(EditAnywhere,Category = "Inventory")
	TMap<EContainerTags,float> InventoryTags; 

	UPROPERTY(ReplicatedUsing="OnRep_Content")
	FContentArray Content;
	
	UPROPERTY(EditAnywhere,Category = "Inventory")
	TArray<FSerializedInventorySlot> DefaultItems;

	UPROPERTY(EditDefaultsOnly,Category = "Inventory")
	TArray<EInventoryFeatures> Features;
private:
	
	UPROPERTY()
	UDataTable* DT_Items;

	UPROPERTY()
	UDataTable* DT_Equipment;

	UPROPERTY()
	TArray<FSerializedInventorySlot> LocalPreviousItems;

	
/** FUNCTIONS */


public:

	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(BlueprintCallable,BlueprintPure)
	TArray<FSerializedInventorySlot>& GetItems()
	{
		return Content.Items;
	}
	
	UFUNCTION(BlueprintCallable,Category="Inventory")
	void SetItems(const TArray<FSerializedInventorySlot>& InItems)
	{
		Content.Items = InItems;
		Content.MarkArrayDirty();
		GetOwner()->ForceNetUpdate();
	};
	
	UFUNCTION(BlueprintCallable,Category="Inventory|Replication")
	void MarkInventorySlotDirty(const int Index)
	{
		GetOwner()->ForceNetUpdate();
		Content.MarkItemDirty(Content.Items[Index]);
	}

	UFUNCTION(BlueprintCallable,Category="Inventory|Replication")
	void EditSlotTag(const int Index,const EItemTags Tag,const FName NewValue);
	
	UFUNCTION(Server,BlueprintCallable,Category="Inventory",Unreliable)
	void ServerMoveItem(UInventoryComponent* DestinationInventory,const int DestinationIndex,const int SourceIndex ,const int Divider,const int SourceSlotContentIndex = -1,const int DestinationSlotContentIndex = -1);

	UFUNCTION(BlueprintCallable,Category="Inventory")
	int GetItemAmount(const FName& ItemID);
	
	UFUNCTION(BlueprintCallable,Category="Inventory")
	bool EnoughForCraft(const TArray<FRecipeRow> RecipeRows,const int& ItemAmount);

	UFUNCTION(BlueprintCallable,Category="Inventory")
	bool RemoveItemsForCraft(const TArray<FRecipeRow> RecipeRows,const int& ItemAmount);
	
protected: 
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnRep_Content();
	
	void InventoryUpdated();
	
	int FindEmptyIndex() const;

	int FindStackableIndex(const FInventorySlot& ItemToStack) const;

	int GetMaxStackSize(const FName ItemID) const;

	bool IsInnerAcceptable(const FName Outer, const FName ItemID) const;
	
	int AddToStack(const FInventorySlot& NewItem,const int StackableIndex);
	
	int AddToEmptySlot(const FInventorySlot& NewItem,const int EmptySlot);

	void GenerateSlotsUniqueIDs();

public:
	
	UFUNCTION(BlueprintCallable)
	bool AddItem(FInventorySlot NewItem);

	UFUNCTION(BlueprintCallable)
	bool AddItems(TArray<FInventorySlot> NewItems);
};








