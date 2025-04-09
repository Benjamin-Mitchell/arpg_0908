// Copyright Tinzie Games


#include "AbilitySystem/Abilities/ArpgPassiveAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "AbilitySystem/arpgAbilitySystemComponent.h"

void UArpgPassiveAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                          const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (UarpgAbilitySystemComponent* ArpgASC = Cast<UarpgAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo())))
	{
		ArpgASC->DeactivatePassiveAbility.AddUObject(this, &UArpgPassiveAbility::ReceiveDeactivate);
	}

	// Create a delay task that will end the ability after the duration
	if (AbilityDuration > 0.0)
	{
		UAbilityTask_WaitDelay* WaitTask = UAbilityTask_WaitDelay::WaitDelay(this, AbilityDuration);
		WaitTask->OnFinish.AddDynamic(this, &UArpgPassiveAbility::OnDurationFinished);
		WaitTask->ReadyForActivation();
	}
}

void UArpgPassiveAbility::ReceiveDeactivate(const FGameplayTag& AbilityTag)
{
	if (AbilityTags.HasTagExact(AbilityTag))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}

void UArpgPassiveAbility::OnDurationFinished()
{
	// Check if ability is still active
	if (IsActive())
	{
		// End the ability
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
	}
}
