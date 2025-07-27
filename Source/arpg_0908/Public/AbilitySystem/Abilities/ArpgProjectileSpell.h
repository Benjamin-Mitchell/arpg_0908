// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/arpgDamageGameplayAbility.h"
#include "ArpgProjectileSpell.generated.h"

class AArpgProjectile;
/**
 * 
 */
UCLASS()
class ARPG_0908_API UArpgProjectileSpell : public UArpgDamageGameplayAbility
{
	GENERATED_BODY()


protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AArpgProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ProjectileSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool ProjectilesShouldCollide = true;

	UFUNCTION(BlueprintCallable, Category = "Projectile", meta = (AutoCreateRefTerm = "InWithTags,InPassThroughTags,InAvoidActors"))
	void FireProjectile(const FVector& ProjectileTargetLocation, const FGameplayTagContainer& InWithTags, const FGameplayTagContainer& InPassThroughTags,
	const TArray<AActor*> &InAvoidActors, const bool InProjectileDealsDamage,
	UPARAM(meta=(GameplayTagFilter="GameplayEventTagsCategory"))  FGameplayTag MontageSocketLocationTag);

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void FireArcingProjectile(UPARAM(meta=(GameplayTagFilter="GameplayEventTagsCategory")) FGameplayTag MontageSocketLocationTag, const FVector& ProjectileTargetLocation);

};
