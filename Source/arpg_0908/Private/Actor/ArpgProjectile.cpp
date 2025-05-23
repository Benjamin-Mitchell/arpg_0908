// Copyright Tinzie Games


#include "Actor/ArpgProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/ArpgAbilitySystemLibrary.h"
#include "arpg_0908/arpg_0908.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

AArpgProjectile::AArpgProjectile()
{
	bReplicates = true;

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere);
	Sphere->SetCollisionObjectType(ECC_Projectile);


}

void AArpgProjectile::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(LifeSpan);
	
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AArpgProjectile::OnSphereOverlap);
	
	LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound, GetRootComponent());
	
	if (CollisionEnabled)
	{
		Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
		Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
		Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
		Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	}
	else
	{
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AArpgProjectile::OnHit()
{
	if (LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
		LoopingSoundComponent->DestroyComponent();
	}
	if (ImpactSound) UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), GetActorRotation());
	if (ImpactEffect) UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation(), GetActorRotation());
}

void AArpgProjectile::Destroyed()
{
	if (LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
		LoopingSoundComponent->DestroyComponent();
	}
	if(!bHit && !HasAuthority()) //if client hasn't played the Destroy effects yet because server sent the destroy event first 
	{
		OnHit();	
	}
	Super::Destroyed();
}

void AArpgProjectile::DamageTargetASC(AActor* OtherActor)
{
	if (!IsValidOverlap(OtherActor)) return;
	
	if(UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
	{
		const FVector DeathImpulse = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;
		DamageEffectParams.DeathImpulse = DeathImpulse;
		const bool bKnockback = FMath::RandRange(1, 100) < DamageEffectParams.KnockbackChance;

		if (bKnockback)
		{
			//FRotator Rotation = GetActorRotation();

			FRotator Rotation = (OtherActor->GetActorLocation() - GetActorLocation()).Rotation();
			Rotation.Pitch = DamageEffectParams.KnockbackPitch;
			const FVector KnockbackDirection = Rotation.Vector();
			
			const FVector KnockbackForce = KnockbackDirection * DamageEffectParams.KnockbackForceMagnitude;
			DamageEffectParams.KnockbackForce = KnockbackForce;
		}
		DamageEffectParams.TargetAbilitySystemComponent = TargetASC;

		UArpgAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
	}
}

void AArpgProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{	
	if (!IsValidOverlap(OtherActor)) return;

	if(!bHit) OnHit();

	if(HasAuthority())
	{
		OnDestroy(OtherActor);
		Destroy();
	}
	else
	{
		bHit = true;
	}
}

bool AArpgProjectile::IsValidOverlap(AActor* OtherActor)
{
	if (DamageEffectParams.SourceAbilitySystemComponent == nullptr) return false;
	AActor* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();
	if (SourceAvatarActor == OtherActor) return false;
	if (!UArpgAbilitySystemLibrary::IsNotFriendBasedOnTag(SourceAvatarActor, OtherActor)) return false;

	return true;
}

