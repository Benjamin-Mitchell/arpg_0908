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
#include "Net/UnrealNetwork.h"

AArpgProjectile::AArpgProjectile()
{
	bReplicates = true;

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere);
	Sphere->SetCollisionObjectType(ECC_Projectile);


}

void AArpgProjectile::SetCollisionTags(const FGameplayTagContainer& InWithTags, const FGameplayTagContainer& InPassThroughTags,
	const TArray<AActor*> &InAvoidActors)
{
	//These are all replicated variables, and should only be assigned if the info is useful
	
	if (!InWithTags.IsEmpty())
		WithTags = InWithTags;
	if (!InPassThroughTags.IsEmpty())
		PassThroughTags = InPassThroughTags;
	if (InAvoidActors.Num() > 0)
		AvoidActors = InAvoidActors;
}

void AArpgProjectile::Fire()
{
}

void AArpgProjectile::BeginPlay()
{
	Super::BeginPlay();
	if (LifeSpan > 0.0f)
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
	bool ActorHasValidASC = false;
	if (!IsValidOverlap(OtherActor, ActorHasValidASC)) return;
	
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
	
	bool OtherActorHasASC = true;
	if (!IsValidOverlap(OtherActor, OtherActorHasASC)) return;

	if(!bHit) OnHit();

	if(HasAuthority())
	{
		//We assume all valid targets have an ASC, otherwise its blocking geometry.
		if (OtherActorHasASC)
			CustomOnDestroyLogic(OtherActor);
		Destroy();
	}
	else
	{
		bHit = true;
	}
}

bool AArpgProjectile::IsValidOverlap(AActor* OtherActor, bool& OutOtherActorHasASC)
{
	if(UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
	{
		OutOtherActorHasASC = true;
		//If we don't have any of the WithTags, fail overlap. (NOTE: if there are no With Tags, it will always pass overlap)
		if (!TargetASC->HasAnyMatchingGameplayTags(WithTags))
			return false;

		//If we have any of the PassThroughTags, we fail.
		if (TargetASC->HasAnyMatchingGameplayTags(PassThroughTags))
			return false;
		
		if (AvoidActors.Contains(OtherActor))
			return false;
	}
	else
	{
		OutOtherActorHasASC = false;
		return true;
	}

	//if (!UArpgAbilitySystemLibrary::IsNotFriendBasedOnTag(SourceAvatarActor, OtherActor)) return false;

	return true;
}

void AArpgProjectile::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AArpgProjectile, WithTags);
	DOREPLIFETIME(AArpgProjectile, PassThroughTags);
	DOREPLIFETIME(AArpgProjectile, AvoidActors);
}

