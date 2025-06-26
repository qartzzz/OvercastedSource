//Copyright OVERCASTED, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterBase.h"
#include "InputAction.h"
#include "Camera/CameraComponent.h"
#include "Components/Equipment/Equipment.h"
#include "Components/Expenses/ExpensesComponent.h"
#include "Components/Inventory/InventoryComponent.h"
#include "Engine/StreamableManager.h"
#include "Interfaces/ICharacterInfo.h"
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
class OVERCASTEDREVAMP_API APlayerCharacter : public ACharacterBase , public IICharacterInfo
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

	UPROPERTY(BlueprintReadOnly,Replicated)
	uint8 CurrentSlot = 255;
	
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
	UStaticMeshComponent* ThirdPersonMagazine;
	
	UPROPERTY(Category= "Character",EditDefaultsOnly,BlueprintReadWrite,meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FirstPersonWeapon;

	UPROPERTY(Category= "Character",EditDefaultsOnly,BlueprintReadWrite,meta = (AllowPrivateAccess = "true"))
	USceneComponent* AimDownSightsPoint;
	
	UPROPERTY(Category= "Character",EditDefaultsOnly,BlueprintReadWrite,meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* FirstPersonMagazine;
	
	UPROPERTY(Category= "Components",EditDefaultsOnly,BlueprintReadWrite,meta = (AllowPrivateAccess = "true"))
	UInventoryComponent* Inventory;

	UPROPERTY(Category= "Components",EditDefaultsOnly,BlueprintReadWrite,meta = (AllowPrivateAccess = "true"))
	UInventoryComponent* EquipmentInventory;
	
	UPROPERTY(Category= "Components",EditDefaultsOnly,BlueprintReadWrite,meta = (AllowPrivateAccess = "true"))
	UExpensesComponent* Expenses;

	UPROPERTY(Category= "Components",EditDefaultsOnly,BlueprintReadWrite,meta = (AllowPrivateAccess = "true"))
	UEquipment* Equipment;
	
	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual float InternalTakePointDamage(float Damage, struct FPointDamageEvent const& PointDamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	void JumpDelay();
	
	UFUNCTION(Server, Unreliable)
	void ServerChangeSlot(const uint8 Slot);


protected:
	
	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
	void Move(const FInputActionValue& Value);
	
	void Look(const FInputActionValue& Value);

	void ActionJump(const FInputActionValue& Value);

	void ChangeSlots();

	void ChangeSlot(const uint8 Slot);
	
	void Interact();
	
    UFUNCTION(Server, Unreliable)
	void SR_Interact();

	void Attack();

	void AttackReleased();
	
	UFUNCTION(Server, Unreliable)
	void SR_Attack(const bool Released);

	void SecondAttack();

	void SecondAttackReleased();
	
	UFUNCTION(Server, Unreliable)
	void SR_SecondAttack(const bool Released);

	void Reload();

	UFUNCTION(Server, Unreliable)
	void SR_Reload();
	
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput")
	UInputMappingContext* InputMapping;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput")
	UInputAction* InputActionLook;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput")
	UInputAction* InputActionMove;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput")
	UInputAction* InputActionJump;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput")
	UInputAction* InputActionSlots;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput")
	UInputAction* InputActionCrouch;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput")
	UInputAction* InputActionInteract;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput")
	UInputAction* InputActionAttack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput")
	UInputAction* InputActionSecondAttack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput")
	UInputAction* InputActionReload;
	
	
	UFUNCTION()
	void OnComponentLoaded();
	
	TSharedPtr<FStreamableHandle> StreamableHandle;
private:
	UPROPERTY()
	UActorComponent* CurrentWeapon = nullptr;

	uint8 CurrentSlotUniqueID = 255;

	void SetHealth(const float NewHealth);
	
	UFUNCTION()
	void IsWeaponChanged(const TArray<int>& ChangedIndexes);

	UFUNCTION()
	void OnRep_CharacterState();

	void DestroyWeapon();
	
	void ChangeWeapon(const FSerializedInventorySlot& Item);

	void OnKilled();

	void OnWounded();

	void OnBasic() const;

	const FEquipmentProtection GetEquipmentProtection_Implementation() override;
};



