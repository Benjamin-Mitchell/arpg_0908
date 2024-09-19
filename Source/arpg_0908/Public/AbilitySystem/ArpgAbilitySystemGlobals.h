// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "ArpgAbilitySystemGlobals.generated.h"

struct FArpgGameplayEffectContext;
/**
 * 
 */
UCLASS()
class ARPG_0908_API UArpgAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()

	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
};
