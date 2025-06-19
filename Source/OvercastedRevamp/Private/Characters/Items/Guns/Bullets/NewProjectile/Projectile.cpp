// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Items/Guns/Bullets/NewProjectile/Projectile.h"
#include "Characters/Items/Guns/Bullets/ProjectileBase.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AProjectile::AProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	ProjectMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));

	DT_Impacts = ConstructorHelpers::FObjectFinder<UDataTable>(TEXT("/Game/Items/Weapons/Projectile/DT_ProjectileImpacts.DT_ProjectileImpacts")).Object;
}


// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwner())
	{
		APlayerController* PC = Cast<APlayerController>(GetOwner());
		if (PC->GetPawn())
		{
			OwnerPawn = PC->GetPawn();
			ProjectMovementComponent->Velocity += OwnerPawn->GetVelocity();
		}
	}

	if (HasAuthority())
	{
		const FVector StartLocation = GetActorLocation();
		const FVector EndLocation = StartLocation + GetActorForwardVector() * ProjectMovementComponent->InitialSpeed / 30;
		
		TArray<AActor*> IgnoreActors;
		IgnoreActors.Add(this);
		IgnoreActors.Add(OwnerPawn);
		
		FHitResult Hit;
		
		UKismetSystemLibrary::SphereTraceSingle(GetWorld(),StartLocation,EndLocation,ProjectileRadius,TraceTypeQuery1,false,IgnoreActors,EDrawDebugTrace::None,Hit,true);
		
		if (Hit.bBlockingHit && !WasHit)
		{
			WasHit = true;
			if (IsValid(Hit.GetActor()))
			{
				FDamageEvent DamageEvent;
				
				AController* Controller = IsValid(GetOwner()) ? GetOwner()->GetInstigatorController() : nullptr;
				DamageEvent.DamageTypeClass = DamageType;
				
				Hit.GetActor()->TakeDamage(DamageAmount,DamageEvent,Controller,GetOwner());
				GEngine->AddOnScreenDebugMessage(-1,5,FColor::Red,"Hit");
			} 
			Destroy();
		}
	}
	else
	{
		const FVector StartLocation = GetActorLocation();
		const FVector EndLocation = StartLocation + GetActorForwardVector() * ProjectMovementComponent->InitialSpeed / 30;
		TArray<AActor*> IgnoreActors;
		IgnoreActors.Add(this);
		IgnoreActors.Add(OwnerPawn);
		
		FHitResult Hit;
		
	    UKismetSystemLibrary::SphereTraceSingle(GetWorld(),StartLocation,EndLocation,ProjectileRadius,TraceTypeQuery1,false,IgnoreActors,EDrawDebugTrace::None,Hit,true);
		
		if (Hit.bBlockingHit && !WasHit)
		{
			WasHit = true;
			GEngine->AddOnScreenDebugMessage(-1,5,FColor::Red,"ClientHit");
			if (DT_Impacts->GetRowNames().Contains(Hit.PhysMaterial->GetName()))
			{ 
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Hit.PhysMaterial->GetName());
				const FProjectileImpacts* ImpactFX = DT_Impacts->FindRow<FProjectileImpacts>(FName(Hit.PhysMaterial.Get()->GetFName()),"");
				
				constexpr EAttachLocation::Type LocationType = EAttachLocation::KeepWorldPosition;
				
				UGameplayStatics::SpawnEmitterAttached(ImpactFX->ImpactParticle,Hit.Component.Get(),"",Hit.Location,GetActorForwardVector().Rotation(),FVector(1,1,1),LocationType);
				
				UGameplayStatics::SpawnDecalAttached(ImpactFX->ImpactMaterial,ImpactFX->DecalSize,Hit.Component.Get(),"",Hit.Location,GetActorForwardVector().Rotation(),LocationType);

				UGameplayStatics::SpawnSoundAtLocation(GetWorld(),ImpactFX->ImpactSound,Hit.Location,GetActorForwardVector().Rotation());
			}
		}
	}
	
}


// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    ProjectileTick(DeltaTime);
}

void AProjectile::ProjectileTick(float DeltaTime)
{
	if (HasAuthority())
	{
		const FVector StartLocation = GetActorLocation();
		const FVector EndLocation = StartLocation + (GetVelocity() * DeltaTime + 0.05f);
	
		TArray<AActor*> IgnoreActors;
		IgnoreActors.Add(this);
		IgnoreActors.Add(OwnerPawn);
		
		FHitResult Hit;
		
		UKismetSystemLibrary::SphereTraceSingle(GetWorld(),StartLocation,EndLocation,ProjectileRadius,TraceTypeQuery1,false,IgnoreActors,EDrawDebugTrace::ForDuration,Hit,true);
		
		if (Hit.bBlockingHit && !WasHit)
		{
			WasHit = true;
			if (IsValid(Hit.GetActor()))
			{
				FDamageEvent DamageEvent;
				
				AController* Controller = IsValid(GetOwner()) ? GetOwner()->GetInstigatorController() : nullptr;
				DamageEvent.DamageTypeClass = DamageType;
				
				Hit.GetActor()->TakeDamage(DamageAmount,DamageEvent,Controller,GetOwner());
			} 
			Destroy();
		}
	}
	else
	{
		const FVector StartLocation = GetActorLocation();
		const FVector EndLocation = StartLocation + GetVelocity() * DeltaTime + 0.05f;
		
		TArray<AActor*> IgnoreActors;
		IgnoreActors.Add(this);
		IgnoreActors.Add(OwnerPawn);
		
		FHitResult Hit;
		
		UKismetSystemLibrary::SphereTraceSingle(GetWorld(),StartLocation,EndLocation,ProjectileRadius,TraceTypeQuery1,false,IgnoreActors,EDrawDebugTrace::None,Hit,true);
		
		
		if (Hit.bBlockingHit && !WasHit)
		{
			WasHit = true;
			
			if (DT_Impacts->GetRowNames().Contains(Hit.PhysMaterial->GetName()))
			{ 
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Hit.PhysMaterial->GetName());
				const FProjectileImpacts* ImpactFX = DT_Impacts->FindRow<FProjectileImpacts>(FName(Hit.PhysMaterial.Get()->GetFName()),"");
				
				constexpr EAttachLocation::Type LocationType = EAttachLocation::KeepWorldPosition;
				
				UGameplayStatics::SpawnEmitterAttached(ImpactFX->ImpactParticle,Hit.Component.Get(),"",Hit.Location,GetActorForwardVector().Rotation(),FVector(1,1,1),LocationType);
				
				UGameplayStatics::SpawnDecalAttached(ImpactFX->ImpactMaterial,ImpactFX->DecalSize,Hit.Component.Get(),"",Hit.Location,GetActorForwardVector().Rotation(),LocationType);

				UGameplayStatics::SpawnSoundAtLocation(GetWorld(),ImpactFX->ImpactSound,Hit.Location,GetActorForwardVector().Rotation());
			}
		}
	}
}

