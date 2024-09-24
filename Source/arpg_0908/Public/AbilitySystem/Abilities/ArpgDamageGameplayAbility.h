// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/arpgGameplayAbility.h"
#include "ArpgDamageGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class ARPG_0908_API UArpgDamageGameplayAbility : public UarpgGameplayAbility
{
	GENERATED_BODY()

public:


protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	TMap<FGameplayTag, FScalableFloat> DamageTypes;

};
