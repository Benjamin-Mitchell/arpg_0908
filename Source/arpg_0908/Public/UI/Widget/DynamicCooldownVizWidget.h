// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UI/Widget/arpgUserWidget.h"
#include "Styling/SlateBrush.h"
#include "DynamicCooldownVizWidget.generated.h"


class UWaitCooldownChange;

USTRUCT(BlueprintType)
struct FAbilityCooldownInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	float RemainingCooldown = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	FSlateBrush IconBrush;

	UPROPERTY(BlueprintReadOnly)
	FSlateBrush BackgroundBrush;
	
};
/**
 * 
 */
UCLASS()
class ARPG_0908_API UDynamicCooldownVizWidget : public UarpgUserWidget
{
	GENERATED_BODY()
	
protected:
	UFUNCTION(BlueprintCallable)
	void AppendInfoOverwrite(const FGameplayTag AbilityInputTag, const FGameplayTag AbilityCooldownTag, float RemainingCooldown, FSlateBrush IconBrush, FSlateBrush BackgroundBrush);

	UFUNCTION(BlueprintCallable)
	void SetCooldownRemaining(const FGameplayTag AbilityCooldownTag, float RemainingCooldown);

	UFUNCTION(BlueprintCallable)
	FAbilityCooldownInfo GetAbilityCooldownInfo(FGameplayTag InCooldownTag);
	
	UFUNCTION(BlueprintCallable)
	FGameplayTag GetInputTagFromCooldownTag(const FGameplayTag InInputTag);

	UFUNCTION(BlueprintCallable)
	void CustomTickDeductCounters(float InDeltaTime);

	UPROPERTY(BlueprintReadWrite)
	TMap<FGameplayTag, UWaitCooldownChange*> CooldownTagToWaitCooldownAsyncTasks;
	
	TMap<FGameplayTag, FGameplayTag> InputTagToCooldownTags;
	
	TMap<FGameplayTag, FAbilityCooldownInfo> AbilityCooldownInfos;
};
