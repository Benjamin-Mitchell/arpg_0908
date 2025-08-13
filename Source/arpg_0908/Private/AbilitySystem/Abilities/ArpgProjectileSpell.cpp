// Copyright Tinzie Games


#include "AbilitySystem/Abilities/ArpgProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Actor/ArpgProjectile.h"
#include "Interaction/CombatInterface.h"
#include "ArpgGameplayTags.h"
#include "Projects.h"
#include "Actor/ArcingProjectile.h"

void UArpgProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);	
}

AArpgProjectile* UArpgProjectileSpell::SpawnProjectile(const FVector& ProjectileFacingLocation, const FGameplayTagContainer& InWithTags, const FGameplayTagContainer& InPassThroughTags,
	const TArray<AActor*> &InAvoidActors, const bool InProjectileDealsDamage, FGameplayTag MontageSocketLocationTag)
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if(!bIsServer) return nullptr;

	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
	if(CombatInterface)
	{
		const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(
			GetAvatarActorFromActorInfo(),
			MontageSocketLocationTag);
		
		FRotator Rotation = (ProjectileFacingLocation - SocketLocation).Rotation();
		Rotation.Pitch = 0.f; //ensure the projectile moves parallel to the floor, at the height of the firing point.

		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(Rotation.Quaternion());
		
		AArpgProjectile* Projectile = GetWorld()->SpawnActorDeferred<AArpgProjectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		//These only take effect on the server.
		if (InProjectileDealsDamage)
			Projectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults(nullptr);

		//Server-Only
		Projectile->SetSpawnTransform(SpawnTransform);
		
		//Replicated
		Projectile->SetCollisionTags(InWithTags, InPassThroughTags, InAvoidActors);

		Projectile->FinishSpawning(SpawnTransform);

		return Projectile;
	}
	return nullptr;
}

void UArpgProjectileSpell::FireArcingProjectile(UPARAM(meta=(GameplayTagFilter="GameplayEventTagsCategory")) FGameplayTag MontageSocketLocationTag, const FVector& ProjectileTargetLocation)
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if(!bIsServer)
	{
		return;
	}

	
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
	if(CombatInterface)
	{
		const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(
			GetAvatarActorFromActorInfo(),
			MontageSocketLocationTag);
		
		FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
 
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(Rotation.Quaternion());
		
		AArpgProjectile* Projectile = GetWorld()->SpawnActorDeferred<AArpgProjectile>(
		ProjectileClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(GetOwningActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		AArcingProjectile* ArcingProjectile = CastChecked<AArcingProjectile>(Projectile);
		
		FArcingProjectileInitData InitData;
		InitData.ArcTargetLocation = ProjectileTargetLocation;
		InitData.ProjectileSpeed = ProjectileSpeed;
		
		ArcingProjectile->InitData = InitData;
		
		//TODO: this should be in the actor blueprint, not here (it will only do anything on the server anyway)
		ArcingProjectile->CollisionEnabled = ProjectilesShouldCollide;

		//These only take effect on the server.
		ArcingProjectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults(nullptr);
		Projectile->SetSpawnTransform(SpawnTransform);
		
		ArcingProjectile->FinishSpawning(SpawnTransform);
		
	}
}
