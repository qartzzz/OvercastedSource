//Copyright OVERCASTED, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ExpenseAffects/ExpenseAffectBase.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "ExpensesComponent.generated.h"

  

USTRUCT(BlueprintType)
struct FAffects
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UExpenseAffectBase> AffectClass;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	float DefaultAmount;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	float MaxAmount;
	UPROPERTY(EditDefaultsOnly)
	float RemovePerSecond;
	FAffects() : AffectClass(UExpenseAffectBase::StaticClass()),DefaultAmount(0),MaxAmount(0), RemovePerSecond(0) {}
};

UENUM(Blueprintable)
enum class EExpenses : uint8
{
	Food UMETA(DisplayName = "Food"),
	Water UMETA(DisplayName = "Water"),
	Radiation UMETA(DisplayName = "Radiation"),
	Poison UMETA(DisplayName = "Poison"),
	Bleeding UMETA(DisplayName = "Bleeding")
}; 

struct FExpensesArray;
USTRUCT(BlueprintType)
struct FExpense : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly,EditDefaultsOnly)
	EExpenses Expense;
	UPROPERTY(BlueprintReadOnly,EditDefaultsOnly)
	float Value;

	void PreReplicatedRemove(const FExpensesArray &InArraySerializer);
	void PostReplicatedAdd(const FExpensesArray &InArraySerializer);
	void PostReplicatedChange(const FExpensesArray &InArraySerializer);
	
	FExpense(EExpenses Expense = EExpenses::Radiation,float Value = 0) : Expense(Expense), Value(Value)
	{
		 
	}
};

USTRUCT(BlueprintType)
struct FExpensesArray : public FFastArraySerializer
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TArray<FExpense> Expenses;
	
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FExpense,FExpensesArray>(Expenses, DeltaParms,*this);
	}
	
};

template<>
struct TStructOpsTypeTraits<FExpensesArray> : public TStructOpsTypeTraitsBase2<FExpensesArray>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OVERCASTEDREVAMP_API UExpensesComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
public:	
	UExpensesComponent();
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExpensesChanged);
    UPROPERTY(BlueprintAssignable)
	FOnExpensesChanged OnExpensesChanged;
	
	UFUNCTION(BlueprintCallable,Category="Expenses")
	float GetExpenseValue(const EExpenses Expense);

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Expenses")
	TMap<EExpenses, FAffects> ExpensesSettings;

	UPROPERTY(BlueprintReadWrite,Replicated,Category = "Expenses")
	FExpensesArray ExpensesArray;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	void AddExpense(EExpenses Expense,float Value);
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void ExpensesOutcome();

	float GetOwnerProtection(EExpenses Expense);
};
