// Copyright Tinzie Games


#include "UI/Widget/DynamicCooldownVizWidget.h"

#include "arpg_0908/ArpgLogChannels.h"



void UDynamicCooldownVizWidget::AppendInfoOverwrite(const FGameplayTag AbilityInputTag, const FGameplayTag AbilityCooldownTag, float RemainingCooldown,
                                                    FSlateBrush IconBrush, FSlateBrush BackgroundBrush)
{
	if (FAbilityCooldownInfo* Found = AbilityCooldownInfos.Find(AbilityCooldownTag))
	{
		Found->RemainingCooldown = RemainingCooldown;
		Found->IconBrush = IconBrush;
		Found->BackgroundBrush = BackgroundBrush;
	}
	else
	{
		FAbilityCooldownInfo Info = {RemainingCooldown, IconBrush, BackgroundBrush};
		AbilityCooldownInfos.Add(AbilityCooldownTag, Info);
	}

	if (FGameplayTag* FoundCooldownTag = InputTagToCooldownTags.Find(AbilityInputTag))
	{
		*FoundCooldownTag = AbilityCooldownTag;
	}
	else
	{
		InputTagToCooldownTags.Add(AbilityInputTag, AbilityCooldownTag);
	}
	
}

void UDynamicCooldownVizWidget::SetCooldownRemaining(const FGameplayTag AbilityCooldownTag, float RemainingCooldown)
{
	if (FAbilityCooldownInfo* Found = AbilityCooldownInfos.Find(AbilityCooldownTag))
	{
		Found->RemainingCooldown = RemainingCooldown;
	}
	else
	{
		UE_LOG(LogArpg, Error, TEXT("Can't find info for AbilityTag [%s] in cooldowns %hs"), *AbilityCooldownTag.ToString(), __FUNCTION__);
	}
}

FAbilityCooldownInfo UDynamicCooldownVizWidget::GetAbilityCooldownInfo(FGameplayTag InCooldownTag)
{
	if (!AbilityCooldownInfos.Contains(InCooldownTag))
	{
		UE_LOG(LogArpg, Error, TEXT("Requested invalid Cooldown Tag (%s) in  %hs"), *InCooldownTag.ToString(), __FUNCTION__);
		return FAbilityCooldownInfo();
	}
		
	return AbilityCooldownInfos[InCooldownTag];
}

FGameplayTag UDynamicCooldownVizWidget::GetInputTagFromCooldownTag(const FGameplayTag InInputTag)
{
	if (!InputTagToCooldownTags.Contains(InInputTag))
	{
		UE_LOG(LogArpg, Error, TEXT("Requested invalid Input Tag (%s) in  %hs"), *InInputTag.ToString(), __FUNCTION__);
		return FGameplayTag();
	}
	
	return InputTagToCooldownTags[InInputTag];
}

void UDynamicCooldownVizWidget::CustomTickDeductCounters(float InDeltaTime)
{
	for (auto It = AbilityCooldownInfos.CreateIterator(); It; ++It)
	{
		if (It->Value.RemainingCooldown >= 0.0)
			It->Value.RemainingCooldown = It->Value.RemainingCooldown - InDeltaTime;
	}
}
