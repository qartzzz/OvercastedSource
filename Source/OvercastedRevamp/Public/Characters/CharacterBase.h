//Copyright OVERCASTED, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/DamageTypes/DamageTypeBase.h"
#include "GameFramework/Character.h"
#include "Net/Core/PushModel/PushModel.h"
#include "CharacterBase.generated.h" 


enum class EItemAnimations : uint8;

UCLASS()
class OVERCASTEDREVAMP_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ACharacterBase();

protected:
	virtual void GetLifetimeReplicatedProps(TArray < FLifetimeProperty >& OutLifetimeProps) const override;

	virtual void BeginPlay() override;

	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(Replicated,BlueprintReadOnly)
	EItemAnimations CurrentAnimation;

	UFUNCTION(BlueprintCallable,Category = "Character | Animation")
	void SetCurrentAnimation(const EItemAnimations Animation);
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Character | Health")
	float MaxHealth = 100;

	UPROPERTY(EditDefaultsOnly, Category = "Character | Specials")
	EObjectTypes PawnType = EObjectTypes::PawnWeak;

	UPROPERTY(Replicated,BlueprintReadOnly, Category = "Character | Health")
	float Health = 100;
	
	float PreviousHealth = 100;
	
	UFUNCTION(BlueprintImplementableEvent,BlueprintCosmetic)
	void HealthChanged(float PreviousHealth_, float CurrentHealth, FVector CauserLocation);

	UFUNCTION(Client,Unreliable)
	void ClientHealthChanged(FVector CauserLocation);
	
};

inline void ACharacterBase::SetCurrentAnimation(const EItemAnimations Animation)
{
	if (CurrentAnimation != Animation)
	{
		ForceNetUpdate();
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass,CurrentAnimation,this);
		CurrentAnimation = Animation;
	}
}
