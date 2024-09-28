// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/arpgGameplayAbility.h"
#include "HookAbility.generated.h"

class AArpgProjectile;
/**
 * 
 */
UCLASS()
class ARPG_0908_API UHookAbility : public UarpgGameplayAbility
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	//For now this is also an ArpgProjectile, much like the base ProjectileSpell class.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AArpgProjectile> ProjectileClass;

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void FireProjectile(const FVector& ProjectileTargetLocation);
};
