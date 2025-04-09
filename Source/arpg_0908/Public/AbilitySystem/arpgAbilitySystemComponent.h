// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "arpgAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /*Asset Tags*/)

DECLARE_MULTICAST_DELEGATE_OneParam(FDeactivatePassiveAbility, const FGameplayTag& /*Ablity Tag*/)

/**
 * 
 */
UCLASS()
class ARPG_0908_API UarpgAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	void AbilityActorInfoSet();

	FEffectAssetTags EffectAssetTags;
	FDeactivatePassiveAbility DeactivatePassiveAbility;

	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>> Abilities, bool ActivateImmediately = false);
	void RemoveCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>> Abilities);

	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	
protected:

	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);

private:

	TMap<FGameplayTag,  FGameplayAbilitySpecHandle> OwnedAbilities;
};
