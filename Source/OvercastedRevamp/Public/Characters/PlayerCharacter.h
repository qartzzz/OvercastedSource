//Copyright OVERCASTED, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterBase.h"
#include "InputAction.h"
#include "Camera/CameraComponent.h"
#include "Components/Inventory/InventoryComponent.h"
#include "Engine/StreamableManager.h"
#include "PlayerCharacter.generated.h"

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	Basic UMETA(DisplayName = "Basic"),
	Wounded UMETA(DisplayName = "Wounded"),
	Killed UMETA(DisplayName = "Killed")
};
class UInputMappingContext;
/**
 * 
 */
UCLASS()
class OVERCASTEDREVAMP_API APlayerCharacter : public ACharacterBase
{
	GENERATED_BODY()

	
public:
	APlayerCharacter();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterStateChanged, ECharacterState, NewState);
	UPROPERTY(BlueprintAssignable)
	FOnCharacterStateChanged OnCharacterStateChanged;
	
	UPROPERTY(BlueprintReadOnly,Category = "Character | State",ReplicatedUsing = "OnRep_CharacterState")
	ECharacterState CharacterState;
	
	virtual void Tick(float DeltaSeconds) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	
private:
	bool CanJump = true;

	float SpeedMultiplier = 1;
	
	float PreviousZLocation;

	float ZLocation;

	FVector PreviousVelocity = FVector::ZeroVector;

	FTimerHandle FJumpDelayTimer;
	
	UPROPERTY()
	UDataTable* DT_Items;
	
	UPROPERTY(Category= "Character",EditDefaultsOnly,BlueprintReadWrite,meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

	UPROPERTY(Category= "Character",EditDefaultsOnly,BlueprintReadWrite,meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FirstPersonMesh;
	
	UPROPERTY(Category= "Character",EditDefaultsOnly,BlueprintReadWrite,meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ThirdPersonWeapon;
		
	UPROPERTY(Category= "Character",EditDefaultsOnly,BlueprintReadWrite,meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FirstPersonWeapon;

	UPROPERTY(Category= "Components",EditDefaultsOnly,BlueprintReadWrite,meta = (AllowPrivateAccess = "true"))
	UInventoryComponent* Inventory;
	void JumpDelay();
	
	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(Server, Unreliable)
	void ServerChangeSlot(const uint8 Slot);


protected:
	
	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
	void Move(const FInputActionValue& Value);
	
	void Look(const FInputActionValue& Value);

	void ActionJump(const FInputActionValue& Value);

	void ChangeSlots();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput")
	UInputMappingContext* InputMapping;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput")
	UInputAction* InputActionLook;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput")
	UInputAction* InputActionMove;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput")
	UInputAction* InputActionJump;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput")
	UInputAction* InputActionCrouch;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput")
	UInputAction* InputActionInteract;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput")
	UInputAction* InputActionAttack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput")
	UInputAction* InputActionSlots;

	UFUNCTION()
	void OnComponentLoaded();
	
	TSharedPtr<FStreamableHandle> StreamableHandle;
private:
	UPROPERTY()
	UActorComponent* CurrentWeapon = nullptr;
	
	uint8 CurrentSlot = 255;

	uint8 CurrentSlotUniqueID = 255;

	void SetHealth(const float NewHealth);
	
	UFUNCTION()
	void IsWeaponChanged(const TArray<int>& ChangedIndexes);

	UFUNCTION()
	void OnRep_CharacterState();
	
	void ChangeSlot(const uint8 Slot);

	void DestroyWeapon();
	
	void ChangeWeapon(const FSerializedInventorySlot& Item);

	void OnKilled();

	void OnWounded();

	void OnBasic() const;

	
};



