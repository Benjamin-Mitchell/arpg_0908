// Copyright Tinzie Games


#include "AbilitySystem/arpgAbilitySystemComponent.h"

void UarpgAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UarpgAbilitySystemComponent::EffectApplied);
}

void UarpgAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& EffectSpec,
                                                FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);

	for(const FGameplayTag& Tag : TagContainer)
	{
		//TODO: Broadcast the tag to the widget controller
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Tag Name: %s"), *Tag.ToString()));
	}
}
