//Copyright OVERCASTED, Inc. All Rights Reserved.


#include "Characters/PlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Engine/DamageEvents.h"
#include "Net/UnrealNetwork.h"
#include "Components/CapsuleComponent.h"
#include "Net/Core/PushModel/PushModel.h"
#include "InputMappingContext.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "GameFramework/PlayerState.h"
#include "Interfaces/IPlayerWorldInteraction.h"
#include "Interfaces/IWeaponInteraction.h"


APlayerCharacter::APlayerCharacter()
{
	ThirdPersonWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	ThirdPersonWeapon->SetupAttachment(GetMesh(),"hand_r");
	ThirdPersonWeapon->SetOwnerNoSee(true);
	
	Camera = CreateDefaultSubobject<UCameraComponent>(FName("CameraComponent"));
	Camera->SetupAttachment(GetCapsuleComponent());
	Camera->SetRelativeLocation(FVector(0,0,65));
	
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>("FPArms");
	FirstPersonMesh->SetupAttachment(Camera);
	FirstPersonMesh->SetRelativeLocation(FVector(-4.984583,-1.346184,-147.035300));
	FirstPersonMesh->SetRelativeRotation(FRotator(0,-90,0));
	FirstPersonMesh->SetOnlyOwnerSee(true);
	
	FirstPersonWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FPSWeapon"));
	FirstPersonWeapon->SetupAttachment(FirstPersonMesh,"hand_r");
	FirstPersonWeapon->SetOnlyOwnerSee(true);

	DT_Items = ConstructorHelpers::FObjectFinder<UDataTable>(TEXT("/Game/Items/DT_Items.DT_Items")).Object;
	
	Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));

	Equipment = CreateDefaultSubobject<UInventoryComponent>(TEXT("Equipment"));

	Expenses = CreateDefaultSubobject<UExpensesComponent>(TEXT("Expenses"));
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		Inventory->OnItemsChanged.AddUniqueDynamic(this,&ThisClass::IsWeaponChanged);
	}
	
}

void APlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (GetCharacterMovement()->Velocity.Z >= -5 && PreviousVelocity.Z < -800)
	{
		const FDamageEvent DamageEvent;
		TakeDamage(abs((PreviousVelocity.Z/300) * (PreviousVelocity.Z/300) * (PreviousVelocity.Z/300)),DamageEvent,nullptr,nullptr);
	}
	
	PreviousVelocity = GetCharacterMovement()->Velocity;
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	FDoRepLifetimeParams ParamsBasic;
	ParamsBasic.bIsPushBased = true;
	DOREPLIFETIME_WITH_PARAMS_FAST(APlayerCharacter,CharacterState,ParamsBasic);

	FDoRepLifetimeParams ParamsOwnerOnly;
	ParamsOwnerOnly.bIsPushBased = true;
	ParamsOwnerOnly.Condition = COND_OwnerOnly;
	DOREPLIFETIME_WITH_PARAMS_FAST(APlayerCharacter,CurrentSlot,ParamsOwnerOnly);
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	const APlayerController* PlayerController = Cast<APlayerController>(GetController());
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	
	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(InputMapping, 0);

	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	Input->BindAction(InputActionMove, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
	Input->BindAction(InputActionLook, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
	Input->BindAction(InputActionJump, ETriggerEvent::Triggered, this, &APlayerCharacter::ActionJump);
	
	Input->BindAction(InputActionSlots, ETriggerEvent::Started, this, &APlayerCharacter::ChangeSlots);
	
	Input->BindAction(InputActionInteract, ETriggerEvent::Started, this, &APlayerCharacter::Interact);
	
	Input->BindAction(InputActionAttack, ETriggerEvent::Started, this, &APlayerCharacter::Attack);
	Input->BindAction(InputActionSecondAttack, ETriggerEvent::Started, this, &APlayerCharacter::SecondAttack);
	Input->BindAction(InputActionAttack, ETriggerEvent::Completed, this, &APlayerCharacter::AttackReleased);
	Input->BindAction(InputActionSecondAttack, ETriggerEvent::Completed, this, &APlayerCharacter::SecondAttackReleased);
	Input->BindAction(InputActionReload, ETriggerEvent::Started, this, &APlayerCharacter::Reload);
	
	
	
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D InputVector = Value.Get<FVector2D>();
	AddMovementInput(GetActorRightVector(),FMath::Clamp(InputVector.X * SpeedMultiplier, -0.5,0.5));
	AddMovementInput(GetActorForwardVector(),FMath::Clamp(InputVector.Y * SpeedMultiplier,-0.5,1));
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	const FVector2d LookDirection = Value.Get<FVector2d>();
	if(Controller != nullptr)
	{
		AddControllerYawInput(LookDirection.X);
		AddControllerPitchInput(LookDirection.Y);
	}
}

void APlayerCharacter::ActionJump(const FInputActionValue& Value)
{
	if (!GetWorldTimerManager().IsTimerActive(FJumpDelayTimer) && CanJump)
	{
		GetWorldTimerManager().SetTimer(FJumpDelayTimer,this,&ThisClass::JumpDelay,0.8 + GetVelocity().Length() / 1600);
		Jump();
		CanJump = false;
	}
}

void APlayerCharacter::ChangeSlots()
{
	const APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (IsValid(PlayerController))
	{
		for (FEnhancedActionKeyMapping Mapping : InputMapping->GetMappings())
		{
			if (PlayerController->IsInputKeyDown(Mapping.Key))
			{
				if (IsValid(Cast<UInputModifierScalar>(Mapping.Modifiers[0])))
				{
					ServerChangeSlot(FMath::TruncToInt(Cast<UInputModifierScalar>(Mapping.Modifiers[0])->Scalar.X));
					return;
				}
			}
		}
	}
}

void APlayerCharacter::ChangeSlot(const uint8 Slot)
{
	if (Inventory->Content.Items.IsValidIndex(Slot - 1))
	{
		const FSerializedInventorySlot Item = Inventory->Content.Items[Slot - 1];
		CurrentSlot = Slot - 1;
		CurrentSlotUniqueID = Item.UniqueSlotID;
		
		MARK_PROPERTY_DIRTY_FROM_NAME(APlayerCharacter,CurrentSlot,this);
		
		if (DT_Items->GetRowNames().Contains(Item.Slot.ItemID))
		{
			SetCurrentAnimation(DT_Items->FindRow<FItemData>(Item.Slot.ItemID,"")->ItemAnimation);
		}
		ChangeWeapon(Item);
	}
	else
	{
		DestroyWeapon();
	}
}
 
void APlayerCharacter::ServerChangeSlot_Implementation(const uint8 Slot)
{
	ChangeSlot(Slot);
}

void APlayerCharacter::ChangeWeapon(const FSerializedInventorySlot& Item)
{
	DestroyWeapon(); 
	if (DT_Items->GetRowNames().Contains(Item.Slot.ItemID))
	{
		const TSoftClassPtr<UActorComponent> WeaponClass = DT_Items->FindRow<FItemData>(Item.Slot.ItemID,"")->WeaponClass;
		FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
		FStreamableDelegate Delegate;
		Delegate.BindUObject(this,&ThisClass::OnComponentLoaded);
		StreamableHandle = StreamableManager.RequestAsyncLoad(WeaponClass.ToSoftObjectPath(),Delegate);
	}
	else
	{
		SetCurrentAnimation(EItemAnimations::None);
	}
}

void APlayerCharacter::Interact()
{
	SR_Interact();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Interacting");
}

void APlayerCharacter::SR_Interact_Implementation()
{
	TArray<FHitResult> Hits;
	const FVector Start = Camera->GetComponentLocation();
	const FVector End = Start + GetBaseAimRotation().Vector() * 400;
	FCollisionShape CollisionShape;
	CollisionShape.SetSphere(10);
	
	GetWorld()->SweepMultiByChannel(Hits,Start,End,Camera->GetForwardVector().ToOrientationQuat(),ECC_Visibility,CollisionShape);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Interacting");
	for (const FHitResult& Hit: Hits)
	{
		if (Hit.bBlockingHit && Hit.GetActor() != this)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Hit.GetActor()->GetName());
			if (Hit.GetActor()->GetClass()->ImplementsInterface(UIPlayerWorldInteraction::StaticClass()))
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "SendingMessage");
				IIPlayerWorldInteraction::Execute_Interact(Hit.GetActor(),this->GetPlayerState()->GetPlayerController(),Hit);
				return;	
			}
		}
	}
	
}

void APlayerCharacter::Attack()
{
	SR_Attack(false);
}

void APlayerCharacter::AttackReleased()
{
	SR_Attack(true);
}

void APlayerCharacter::SR_Attack_Implementation(const bool Released)
{
	if (IsValid(CurrentWeapon) && CurrentWeapon->GetClass()->ImplementsInterface(UIWeaponInteraction::StaticClass()))
	{
		IIWeaponInteraction::Execute_PerformAction(CurrentWeapon,Released ? EWeaponAction::AttackRelease : EWeaponAction::Attack);
	}
}

void APlayerCharacter::SecondAttack()
{
	SR_SecondAttack(false);
}

void APlayerCharacter::SecondAttackReleased()
{
	SR_SecondAttack(true);
}

void APlayerCharacter::SR_SecondAttack_Implementation(const bool Released)
{
	if (IsValid(CurrentWeapon) && CurrentWeapon->GetClass()->ImplementsInterface(UIWeaponInteraction::StaticClass()))
	{
		IIWeaponInteraction::Execute_PerformAction(CurrentWeapon,Released ? EWeaponAction::Attack2Release : EWeaponAction::Attack2);
	}
}

void APlayerCharacter::Reload()
{
	SR_Reload();
}

void APlayerCharacter::SR_Reload_Implementation()
{
	if (IsValid(CurrentWeapon) && CurrentWeapon->GetClass()->ImplementsInterface(UIWeaponInteraction::StaticClass()))
	{
		IIWeaponInteraction::Execute_PerformAction(CurrentWeapon,EWeaponAction::Reload);
	}
}


void APlayerCharacter::JumpDelay()
{
	CanJump = true;
}

float APlayerCharacter::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator,AActor* DamageCauser)
{
	if (DamageEvent.DamageTypeClass == UDamageTypeBase::StaticClass())
	{
		UDamageTypeBase* DamageType = Cast<UDamageTypeBase>(DamageEvent.DamageTypeClass.GetDefaultObject());
		SetHealth(Health -= DamageAmount * DamageType->GetDamageMultiplier(PawnType));
	}
	else
	{
		SetHealth(Health - DamageAmount);
	}
	
	if (Health <= 0)
	{
		MARK_PROPERTY_DIRTY_FROM_NAME(APlayerCharacter,CharacterState,this);
		if (CharacterState == ECharacterState::Basic)
		{
			CharacterState = ECharacterState::Wounded;
			OnWounded();
		}
		else
		{
			CharacterState = ECharacterState::Killed;
			OnKilled();
		}
	}
	
	ClientHealthChanged(IsValid(DamageCauser) ? DamageCauser->GetActorLocation() : FVector::ZeroVector);
	return DamageAmount;
}

void APlayerCharacter::OnComponentLoaded()
{
	if (StreamableHandle.IsValid())
	{
		GEngine->AddOnScreenDebugMessage(-1,5,FColor::Red,StreamableHandle->GetLoadedAsset()->GetName());
		CurrentWeapon = AddComponentByClass(Cast<UClass>(StreamableHandle->GetLoadedAsset()),false,FTransform::Identity,false);
	}
}

void APlayerCharacter::SetHealth(const float NewHealth)
{
	ForceNetUpdate();
	MARK_PROPERTY_DIRTY_FROM_NAME(ACharacterBase,Health,this);
	Health = NewHealth;
}

void APlayerCharacter::IsWeaponChanged(const TArray<int>& ChangedIndexes)
{
	GEngine->AddOnScreenDebugMessage(-1,5,FColor::Red,"WeaponChanged");
	if (ChangedIndexes.Contains(CurrentSlot))
	{
		if (Inventory->Content.Items[CurrentSlot].UniqueSlotID != CurrentSlotUniqueID)
		{
			ChangeSlot(CurrentSlot);
		}
	}
}

void APlayerCharacter::DestroyWeapon()
{
	if (IsValid(CurrentWeapon))
	{
		CurrentWeapon->DestroyComponent(); 
		SetCurrentAnimation(EItemAnimations::None);
	}
}

void APlayerCharacter::OnRep_CharacterState()
{
	switch (CharacterState)
	{
	case ECharacterState::Basic:
		OnBasic();
		break;
	case ECharacterState::Wounded:
		OnWounded();
		break;
	case ECharacterState::Killed:
		OnKilled();
		break;
	}
}

void APlayerCharacter::OnKilled()
{
	GetCharacterMovement()->MaxWalkSpeed = 0;
	GetCharacterMovement()->SetJumpAllowed(false);
	Destroy();
}

void APlayerCharacter::OnWounded()
{
	GetCharacterMovement()->MaxWalkSpeed = 0;
	GetCharacterMovement()->SetJumpAllowed(false);
	
	if (HasAuthority())
	{
		SetHealth(40);
	}
	OnCharacterStateChanged.Broadcast(CharacterState);
}

void APlayerCharacter::OnBasic() const
{
	GetCharacterMovement()->MaxWalkSpeed = 500;
	GetCharacterMovement()->SetJumpAllowed(true);

	Camera->AttachToComponent(GetCapsuleComponent(),FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	Camera->SetRelativeLocation(FVector(0,0,65));
}