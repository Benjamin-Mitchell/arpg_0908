// Copyright Tinzie Games


#include "AbilitySystem/Abilities/ArpgProjectileSpell.h"

#include "Kismet/KismetSystemLibrary.h"

void UArpgProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UKismetSystemLibrary::PrintString(this, "Chicken Dinner Activate Ability (C++)", true, true, FLinearColor::Red, 3.f);
}
