// Copyright Tinzie Games


#include "AbilitySystem/arpgAbilitySystemComponent.h"
#include "ArpgGameplayTags.h"
#include "AbilitySystem/Abilities/arpgGameplayAbility.h"

void UarpgAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UarpgAbilitySystemComponent::ClientEffectApplied);
}

void UarpgAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>> Abilities, bool ActivateImmediately)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : Abilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		
		if(const UarpgGameplayAbility* arpgAbility = Cast<UarpgGameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.DynamicAbilityTags.AddTag(arpgAbility->StartupInputTag);
		}

		FGameplayAbilitySpecHandle Handle = AbilitySpec.Handle;

		if (ActivateImmediately)
			GiveAbilityAndActivateOnce(AbilitySpec);
		else
			GiveAbility(AbilitySpec);

		FGameplayTag FirstTag = AbilitySpec.Ability.Get()->AbilityTags.First();
		
		OwnedAbilities.Add(FirstTag, Handle);
		
	}
}

void UarpgAbilitySystemComponent::RemoveCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>> Abilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : Abilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);

		FGameplayTag FirstTag = AbilitySpec.Ability.Get()->AbilityTags.First();
		FGameplayAbilitySpecHandle Handle = OwnedAbilities.FindChecked(FirstTag);
		ClearAbility(Handle);

		OwnedAbilities.Remove(FirstTag);
		//GiveAbilityAndActivateOnce(AbilitySpec);
	}
}

void UarpgAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if(!InputTag.IsValid()) return;

	for(FGameplayAbilitySpec& AbilitySpec: GetActivatableAbilities())
	{
		if(AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if(!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UarpgAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if(!InputTag.IsValid()) return;

	for(FGameplayAbilitySpec& AbilitySpec: GetActivatableAbilities())
	{
		//if(AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (AbilitySpec.IsActive())
			{
				//A
				TArray<UGameplayAbility*> Instances = AbilitySpec.GetAbilityInstances();
				const FGameplayAbilityActivationInfo& ActivationInfo = Instances.Last() -> GetCurrentActivationInfoRef();
				InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, AbilitySpec.Handle, ActivationInfo.GetActivationPredictionKey());
				
				//B
				// UGameplayAbility* PrimaryInstance = AbilitySpec.GetPrimaryInstance();
				// if (PrimaryInstance)
				// {
				// 	InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, AbilitySpec.Handle, PrimaryInstance->GetCurrentActivationInfo().GetActivationPredictionKey());	
				// }
			}
		}
	}
}

void UarpgAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if(!InputTag.IsValid()) return;

	for(FGameplayAbilitySpec& AbilitySpec: GetActivatableAbilities())
	{
		//if(AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			AbilitySpecInputReleased(AbilitySpec);
			if (AbilitySpec.IsActive())
			{
				//A
				TArray<UGameplayAbility*> Instances = AbilitySpec.GetAbilityInstances();
				const FGameplayAbilityActivationInfo& ActivationInfo = Instances.Last() -> GetCurrentActivationInfoRef();
				InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, AbilitySpec.Handle, ActivationInfo.GetActivationPredictionKey());
				
				//B
				// UGameplayAbility* PrimaryInstance = AbilitySpec.GetPrimaryInstance();
				// if (PrimaryInstance)
				// {
				// 	InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, AbilitySpec.Handle, PrimaryInstance->GetCurrentActivationInfo().GetActivationPredictionKey());
				// 	
				// }
				
			}
		}
	}
}

void UarpgAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& EffectSpec,
                                                                     FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);

	EffectAssetTags.Broadcast(TagContainer);
}
