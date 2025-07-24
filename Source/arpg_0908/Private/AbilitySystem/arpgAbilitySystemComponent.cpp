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
	//This is only called on the server, so server logic for OwnedAbilities is handled here and in RemoveCharacterAbilities, but in OnRep_ActivateAbilities for client.
	
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
		
		OwnedAbilities.Add(FirstTag, FOwnedAbilityHandle(AbilitySpec, Handle));
		
	}
	
	bStartupAbilitiesGiven = true;
	AbilitiesChanged.Broadcast(this);
}

void UarpgAbilitySystemComponent::ForEachAbility(const FForEachAbility& Delegate)
{
	//Lock changes to the activatable list
	FScopedAbilityListLock ActiveScopeLock(*this);

	for (FGameplayAbilitySpec DeletedSpec : DeletedAbilitySpecs)
	{
		if (!Delegate.ExecuteIfBound(DeletedSpec, true))
		{
			UE_LOG(LogArpg, Error, TEXT("Failed to execute delegate on Deleted Spec in %hs"), __FUNCTION__);
		}
	}

	TArray<FGameplayAbilitySpec> ActivatableSpecs = GetActivatableAbilities();
	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableSpecs)
	{
		if (!Delegate.ExecuteIfBound(AbilitySpec, false))
		{
			UE_LOG(LogArpg, Error, TEXT("Failed to execute delegate on Active Spec in %hs"), __FUNCTION__);
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
	//This is only called on the client, so client logic for OwnedAbilities is handled here, but in Add/RemoveCharacterAbilities for server.

	
	Super::OnRep_ActivateAbilities();

	//This will run on the clients, where bStartupAbilitiesGiven will still be false. (Will be true on server)
	if (!bStartupAbilitiesGiven)
	{
		bStartupAbilitiesGiven = true;
	}

	//first, go through owned abilities. If any need to be removed, flag for deletion.
	for (TTuple<FGameplayTag, FOwnedAbilityHandle>& Pair : OwnedAbilities)
	{
		bool StillOwned = false;
		for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
		{
			if (Spec.Ability.Get()->AbilityTags.First() == Pair.Key)
			{
				StillOwned = true;
			}
		}

		if (!StillOwned)
			DeletedAbilitySpecs.Add(Pair.Value.Spec);		
	}

	//inefficient but fine
	OwnedAbilities.Empty();
	
	//Next update Owned abilities with any new ones.
	for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		FGameplayAbilitySpecHandle Handle = Spec.Handle;
		
		OwnedAbilities.Add(Spec.Ability.Get()->AbilityTags.First(), FOwnedAbilityHandle(Spec, Handle));
	}
	
	AbilitiesChanged.Broadcast(this);

	DeletedAbilitySpecs.Empty();
}

void UarpgAbilitySystemComponent::RemoveCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>> Abilities)
{
	//This is only called on the server, so server logic for OwnedAbilities is handled here and in AddCharacterAbilities, but in OnRep_ActivateAbilities for client.
	//First flag these for removal.
	for (const TSubclassOf<UGameplayAbility> AbilityClass : Abilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);

		FGameplayTag FirstTag = AbilitySpec.Ability.Get()->AbilityTags.First();

		for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
		{
			if (Spec.Ability.Get()->AbilityTags.First() == FirstTag)
			{
				DeletedAbilitySpecs.Add(Spec);
			}
		}
	}

	//Now actually clear them from the ASC.
	for (const TSubclassOf<UGameplayAbility> AbilityClass : Abilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);

		FGameplayTag FirstTag = AbilitySpec.Ability.Get()->AbilityTags.First();
		FGameplayAbilitySpecHandle Handle = OwnedAbilities.FindChecked(FirstTag).Handle;

		ClearAbility(Handle);

		OwnedAbilities.Remove(FirstTag);
	}

	//Call any delegates that are waiting for owned ability update (even removal)
	AbilitiesChanged.Broadcast(this); //TODO: rename this?
	//And clear the intermediate ToDelete specs.
	DeletedAbilitySpecs.Empty();
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
				if (!TryActivateAbility(AbilitySpec.Handle))
				{
					if (bReleasedPress)
					{
						AbilityActivateFailed.Broadcast(InputTag);
					}
				}
				bReleasedPress = false;
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
