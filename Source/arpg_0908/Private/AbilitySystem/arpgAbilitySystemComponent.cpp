// Copyright Tinzie Games


#include "AbilitySystem/arpgAbilitySystemComponent.h"
#include "ArpgGameplayTags.h"
#include "AbilitySystem/Abilities/arpgGameplayAbility.h"
#include "arpg_0908/ArpgLogChannels.h"

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
	
	bStartupAbilitiesGiven = true;
	AbilitiesGiven.Broadcast(this);
}

void UarpgAbilitySystemComponent::ForEachAbility(const FForEachAbility& Delegate)
{
	//Lock changes to the activatable list
	FScopedAbilityListLock ActiveScopeLock(*this);
	
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!Delegate.ExecuteIfBound(AbilitySpec))
		{
			UE_LOG(LogArpg, Error, TEXT("Failed to execute delegate in %hs"), __FUNCTION__);
		}
	}
}

FGameplayTag UarpgAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability)
	{
		for (FGameplayTag Tag : AbilitySpec.Ability.Get()->AbilityTags)
		{
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities"))))
			{
				return Tag;
			}
		}
	}

	return FGameplayTag();
}

FGameplayTag UarpgAbilitySystemComponent::GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability)
	{
		for (FGameplayTag Tag : AbilitySpec.GetDynamicSpecSourceTags())
		{
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag"))))
			{
				return Tag;
			}
		}
	}
	
	return FGameplayTag();
}

void UarpgAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	//This will run on the clients, where bStartupAbilitiesGiven will still be false. (Will be true on server)
	if (!bStartupAbilitiesGiven)
	{
		bStartupAbilitiesGiven = true;
		AbilitiesGiven.Broadcast(this);
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
				if (TryActivateAbility(AbilitySpec.Handle))
				{
					GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor::Yellow, TEXT("Activation Succeeded"));
				}
				else
				{
					if (bReleasedPress)
					{
						AbilityActivateFailed.Broadcast(InputTag);
						bReleasedPress = false;
					}
					//Widget Controller could bind to a delegate that is broadcast here if we are trying (and failing) a different ability to last time.
					//GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor::Yellow, TEXT("Activation Failed"));

					
				}
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

	bReleasedPress = true;
}

void UarpgAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& EffectSpec,
                                                                     FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);

	EffectAssetTags.Broadcast(TagContainer);
}
