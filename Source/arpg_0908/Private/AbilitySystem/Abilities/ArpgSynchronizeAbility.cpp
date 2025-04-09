// Copyright Tinzie Games


#include "AbilitySystem/Abilities/ArpgSynchronizeAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"


void UArpgSynchronizeAbility::RemoveSynchronizationTagToASC()
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	
	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Avatar))
	{
		ASC->RemoveLooseGameplayTag(SynchronizeTag);
	}
}

void UArpgSynchronizeAbility::AddSynchronizationTagToASC()
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	
	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Avatar))
	{
		ASC->AddLooseGameplayTag(SynchronizeTag);
	}
}
