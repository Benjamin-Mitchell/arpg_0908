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

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void FireProjectile(const FVector& ProjectileTargetLocation);


};
