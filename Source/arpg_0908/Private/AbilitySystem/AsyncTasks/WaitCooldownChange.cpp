// Copyright Tinzie Games


#include "AbilitySystem/AsyncTasks/WaitCooldownChange.h"

#include "AbilitySystemComponent.h"
#include "arpg_0908/ArpgLogChannels.h"

UWaitCooldownChange* UWaitCooldownChange::WaitForCooldownChange(UAbilitySystemComponent* AbilitySystemComponent,
                                                                const FGameplayTag& InCooldownTag)
{
	UWaitCooldownChange* WaitCooldownChange = NewObject<UWaitCooldownChange>();
	WaitCooldownChange->AbilitySystemComponent = AbilitySystemComponent;
	WaitCooldownChange->CooldownTag = InCooldownTag;

	if (!IsValid(AbilitySystemComponent) || !InCooldownTag.IsValid())
	{
		UE_LOG(LogArpg, Error, TEXT("WaitForCooldownChange Initialization failed with tag %s, in Function %hs"), *InCooldownTag.ToString(), __FUNCTION__);
		
		WaitCooldownChange->EndTask();
		return nullptr;
	}

	//To know when a cooldown has ended (tag has been removed)
	AbilitySystemComponent->RegisterGameplayTagEvent(
		InCooldownTag,
		EGameplayTagEventType::NewOrRemoved).AddUObject
		(WaitCooldownChange,
		&UWaitCooldownChange::CooldownTagChanged);

	//To know when a cooldown effect has been applied
	AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(WaitCooldownChange, &UWaitCooldownChange::OnActiveEffectAdded);
	return WaitCooldownChange;
}

void UWaitCooldownChange::EndTask()
{
	if (!IsValid(AbilitySystemComponent)) return;

	AbilitySystemComponent->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);

	SetReadyToDestroy();
	MarkAsGarbage();
}

void UWaitCooldownChange::CooldownTagChanged(const FGameplayTag InCooldownTag, int32 NewCount)
{
	if (NewCount == 0)
	{
		OnCooldownEnded.Broadcast(0.f, InCooldownTag);
	}
}

void UWaitCooldownChange::OnActiveEffectAdded(UAbilitySystemComponent* TargetASC,
	const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer AssetTags;
	SpecApplied.GetAllAssetTags(AssetTags);

	FGameplayTagContainer GrantedTags;
	SpecApplied.GetAllGrantedTags(AssetTags);

	if (AssetTags.HasTagExact(CooldownTag) || GrantedTags.HasTagExact(CooldownTag))
	{
		FGameplayEffectQuery GameplayEffectQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTag.GetSingleTagContainer());
		
		TArray<float> TimesRemaining = AbilitySystemComponent->GetActiveEffectsTimeRemaining(GameplayEffectQuery);

		if (TimesRemaining.Num() > 0)
		{
			float TimeRemaining = TimesRemaining[0];
			for (int32 i = 0; i < TimesRemaining.Num(); i++)
			{
				if (TimesRemaining[i] > TimeRemaining)
				{
					TimeRemaining = TimesRemaining[i];
				}
			}

			OnCooldownBegan.Broadcast(TimeRemaining, CooldownTag);
		}
	}
}
