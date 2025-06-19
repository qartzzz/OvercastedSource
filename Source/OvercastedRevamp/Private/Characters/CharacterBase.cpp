//Copyright OVERCASTED, Inc. All Rights Reserved.
#include "Characters/CharacterBase.h"
#include "Engine/DamageEvents.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"


// Sets default values
ACharacterBase::ACharacterBase()
{
	bNetLoadOnClient = 0;
	SetNetDormancy(ENetDormancy::DORM_Initial);
	SetNetUpdateFrequency(1);
	PrimaryActorTick.bCanEverTick = true;
}


void ACharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams ParamsOwnerOnly;
	ParamsOwnerOnly.bIsPushBased = true;
	ParamsOwnerOnly.RepNotifyCondition = REPNOTIFY_OnChanged;
	DOREPLIFETIME_WITH_PARAMS_FAST(ACharacterBase,Health,ParamsOwnerOnly);
	
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.Condition = COND_None;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;
	DOREPLIFETIME_WITH_PARAMS_FAST(ACharacterBase,CurrentAnimation,Params);
}


void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
}


void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


void ACharacterBase::ClientHealthChanged_Implementation(FVector CauserLocation)
{
	HealthChanged(PreviousHealth, Health, CauserLocation);
	PreviousHealth = Health;
}


float ACharacterBase::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EvenyInstigator, AActor* DamageCauser)
{
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass,Health,this);
	
	if (DamageEvent.DamageTypeClass == UDamageTypeBase::StaticClass())
	{
		UDamageTypeBase* DamageType = Cast<UDamageTypeBase>(DamageEvent.DamageTypeClass.GetDefaultObject());
		Health -= DamageAmount * DamageType->GetDamageMultiplier(PawnType);
	}
	else
	{
		Health -= DamageAmount;
	}
	
	ClientHealthChanged(IsValid(DamageCauser) ? DamageCauser->GetActorLocation() : FVector::ZeroVector);
	
	ForceNetUpdate();
	return DamageAmount;
}

