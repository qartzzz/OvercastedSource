 // Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Items/Guns/Bullets/ProjectileBase.h"

#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AProjectileBase::AProjectileBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetNetDormancy(DORM_Never);
	SetNetCullDistanceSquared(675000000);
    bReplicates = true;
	DT_Impacts = ConstructorHelpers::FObjectFinder<UDataTable>(TEXT("/Game/Items/Weapons/Projectile/DT_ProjectileImpacts.DT_ProjectileImpacts")).Object;
}

// Called when the game starts or when spawned
void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwner())
	{
		OwnerVelocity = Cast<APlayerController>(GetOwner())->GetPawn()->GetVelocity();
	}
}

// Called every frame
void AProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MoveAndTrace(DeltaTime);
}

void AProjectileBase::MoveAndTrace(const float DeltaTime)
{
	if (HasAuthority())
	{
		FlushNetDormancy();
		FHitResult Hit;
		const FVector StartLocation = GetActorLocation();
		const FVector EndLocation = StartLocation + GetActorForwardVector() * Speed * DeltaTime;
		
		FalloffPower += FalloffPower / 10 * DeltaTime;
		
		GEngine->AddOnScreenDebugMessage(-1,5,FColor::Yellow,StartLocation.ToString());
		
		FCollisionQueryParams CQP;
		CQP.AddIgnoredActor(GetOwner());
		CQP.AddIgnoredActor(this);
		
		FCollisionShape CollisionShape;
		CollisionShape.SetSphere(ProjectileRadius);
		GetWorld()->SweepSingleByChannel(Hit,StartLocation + OwnerVelocity * DeltaTime,EndLocation + OwnerVelocity * DeltaTime,FQuat(0,0,0,0),ECC_Visibility,CollisionShape,CQP);
		
		if (Hit.bBlockingHit && !WasHit)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, Hit.Location.ToString());
			WasHit = true;
			if (IsValid(Hit.GetActor()))
			{
				AController* Controller = nullptr;
				FDamageEvent DamageEvent;
				
				Controller = IsValid(GetOwner()) ? GetOwner()->GetInstigatorController() : nullptr;
				DamageEvent.DamageTypeClass = DamageType;
				
				Hit.GetActor()->TakeDamage(DamageAmount,DamageEvent,nullptr,GetOwner());
				GEngine->AddOnScreenDebugMessage(-1,5,FColor::Red,"Hit");
			} 
			Destroy();
		}
		FRotator Rotation = GetActorRotation();
		Rotation.Pitch -= FalloffPower * DeltaTime;
		SetActorRotation(Rotation);
		SetActorLocation(EndLocation);
	}
	else
	{
		FHitResult Hit;
		
		const FVector StartLocation = GetActorLocation();
		const FVector EndLocation = StartLocation + GetActorForwardVector() * Speed * DeltaTime;

		GEngine->AddOnScreenDebugMessage(-1,5,FColor::Blue,StartLocation.ToString());
		
		FalloffPower += FalloffPower / 10 * DeltaTime;
		
		FCollisionQueryParams CQP;
		CQP.AddIgnoredActor(GetOwner());
		CQP.AddIgnoredActor(this);
		CQP.bReturnPhysicalMaterial = true;
		
		FCollisionShape CollisionShape;
		CollisionShape.SetSphere(ProjectileRadius);
		GetWorld()->SweepSingleByChannel(Hit,StartLocation + OwnerVelocity * DeltaTime,EndLocation + OwnerVelocity * DeltaTime,FQuat(0,0,0,0),ECC_Visibility,CollisionShape,CQP);
		
		if (Hit.bBlockingHit && !WasHit)
		{
			WasHit = true;
			
			if (DT_Impacts->GetRowNames().Contains(Hit.PhysMaterial->GetName()))
			{ 
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Hit.PhysMaterial->GetName());
				const FProjectileImpacts* ImpactFX = DT_Impacts->FindRow<FProjectileImpacts>(FName(Hit.PhysMaterial.Get()->GetFName()),"");
				
				EAttachLocation::Type LocationType;
				LocationType = EAttachLocation::KeepWorldPosition;
				
				UGameplayStatics::SpawnEmitterAttached(ImpactFX->ImpactParticle,Hit.Component.Get(),"",Hit.Location,GetActorForwardVector().Rotation(),FVector(1,1,1),LocationType);
				
				UGameplayStatics::SpawnDecalAttached(ImpactFX->ImpactMaterial,ImpactFX->DecalSize,Hit.Component.Get(),"",Hit.Location,GetActorForwardVector().Rotation(),LocationType);

				UGameplayStatics::SpawnSoundAtLocation(GetWorld(),ImpactFX->ImpactSound,Hit.Location,GetActorForwardVector().Rotation());
			}
		}
		FRotator Rotation = GetActorRotation();
		Rotation.Pitch -= FalloffPower * DeltaTime;
		SetActorRotation(Rotation);
		SetActorLocation(EndLocation);
	}
}

