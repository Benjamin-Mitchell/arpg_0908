// Copyright Tinzie Games


#include "AbilitySystem/Abilities/ArpgProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Actor/ArpgProjectile.h"
#include "Interaction/CombatInterface.h"
#include "ArpgGameplayTags.h"
#include "Actor/ArcingProjectile.h"

void UArpgProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);	
}

void UArpgProjectileSpell::FireProjectile(const FVector& ProjectileTargetLocation)
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if(!bIsServer) return;

	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
	if(CombatInterface)
	{
		const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(
			GetAvatarActorFromActorInfo(),
			FArpgGameplayTags::Get().Montage_Attack_Weapon);
		FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
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

		Projectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults(nullptr);

		Projectile->FinishSpawning(SpawnTransform);
	}
}

void UArpgProjectileSpell::FireArcingProjectile(UPARAM(meta=(GameplayTagFilter="GameplayEventTagsCategory")) FGameplayTag SpawnLocationTag, const FVector& ProjectileTargetLocation)
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if(!bIsServer) return;

	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
	if(CombatInterface)
	{
		const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(
			GetAvatarActorFromActorInfo(),
			SpawnLocationTag);
		
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

		ArcingProjectile->ArcTargetLocation = ProjectileTargetLocation;
		ArcingProjectile->ProjectileSpeed = ProjectileSpeed;
		ArcingProjectile->CollidesWithPawns = CollidesWithPawns;
		
		ArcingProjectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults(nullptr);

		ArcingProjectile->FinishSpawning(SpawnTransform);
		
	}
}
