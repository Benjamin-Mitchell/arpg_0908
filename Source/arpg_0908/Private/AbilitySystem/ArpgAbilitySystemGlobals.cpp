// Copyright Tinzie Games


#include "AbilitySystem/ArpgAbilitySystemGlobals.h"

#include "ArpgAbilityTypes.h"

FGameplayEffectContext* UArpgAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FArpgGameplayEffectContext();
}
