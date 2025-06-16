//Copyright OVERCASTED, Inc. All Rights Reserved.

#include "Characters/Components/Expenses/ExpensesComponent.h"
#include "Net/UnrealNetwork.h"
#include "Characters/Components/Expenses/ExpenseAffects/ExpenseAffectBase.h"

void FExpense::PreReplicatedRemove(const FExpensesArray& InArraySerializer)
{
	
}

void FExpense::PostReplicatedAdd(const FExpensesArray& InArraySerializer)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Replicated");
}

void FExpense::PostReplicatedChange(const FExpensesArray& InArraySerializer)
{
	
}

UExpensesComponent::UExpensesComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetComponentTickInterval(3);
	SetIsReplicatedByDefault(true);
}

void UExpensesComponent::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		for (TPair<EExpenses,FAffects> Setting : ExpensesSettings)
		{
			ExpensesArray.Expenses.Add(FExpense(Setting.Key,Setting.Value.DefaultAmount));
		}
		ExpensesArray.MarkArrayDirty();
	}
}

void UExpensesComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (GetOwner())
	{
		if (GetOwner()->HasAuthority())
		{
			for (int i = 0; i != ExpensesArray.Expenses.Num(); i++)
			{
				FExpense& Expense = ExpensesArray.Expenses[i];
				Expense.Value = FMath::Clamp(ExpensesArray.Expenses[i].Value - ExpensesSettings.FindRef(Expense.Expense).RemovePerSecond * DeltaTime,0,ExpensesSettings.FindRef(Expense.Expense).MaxAmount);
				ExpensesArray.MarkItemDirty(Expense);
			}
			ExpensesOutcome();
			GetOwner()->ForceNetUpdate();
		}
		else
		{
			ExpensesOutcome();
		}
	}
}

void UExpensesComponent::AddExpense(EExpenses Expense, float Value)
{
	for (FExpense &Element : ExpensesArray.Expenses)
	{
		if (Element.Expense == Expense)
		{
			Element.Value = FMath::Clamp(Value + Element.Value,0,ExpensesSettings[Expense].MaxAmount);
			ExpensesArray.MarkItemDirty(Element);
			GetOwner()->ForceNetUpdate();
		}
	}
}

void UExpensesComponent::ExpensesOutcome()
{
	if (ExpensesArray.Expenses.Num() > 0)
	{
		for (FExpense Expense : ExpensesArray.Expenses)
		{
			ExpensesSettings.FindRef(Expense.Expense).AffectClass.GetDefaultObject()->Affect(GetOwner(),Expense.Value);
		}	
	}
}  

float UExpensesComponent::GetExpenseValue(const EExpenses Expense)
{
	for (const FExpense Element : ExpensesArray.Expenses)
	{
		if (Element.Expense == Expense)
		{
			return Element.Value;
		}
	} 
	return -1; 
}

void UExpensesComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ThisClass,ExpensesArray,COND_OwnerOnly);
}
