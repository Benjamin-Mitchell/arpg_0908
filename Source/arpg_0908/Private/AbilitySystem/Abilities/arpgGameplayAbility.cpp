// Copyright Tinzie Games


#include "AbilitySystem/Abilities/arpgGameplayAbility.h"

bool UarpgGameplayAbility::GetEndedAlready() const
{
	return EndedAlready;
}

void UarpgGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                      bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (!EndedAlready)
	{
		OnAbilityEnded.Broadcast();
		EndedAlready = true;
	}
}

void UarpgGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	//We reset before calling Super because if there are no Latent actions in the ability, it will call EndAbility before ActivateAbility returns.
	EndedAlready = false;
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

}
