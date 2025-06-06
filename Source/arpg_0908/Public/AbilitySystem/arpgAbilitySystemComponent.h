// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "arpgAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /*Asset Tags*/)

DECLARE_MULTICAST_DELEGATE_OneParam(FAbilitiesGiven, UarpgAbilitySystemComponent* )
DECLARE_MULTICAST_DELEGATE_OneParam(FDeactivatePassiveAbility, const FGameplayTag& /*Ablity Tag*/)
DECLARE_DELEGATE_OneParam(FForEachAbility, const FGameplayAbilitySpec&)

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
	FAbilitiesGiven AbilitiesGiven;
	FDeactivatePassiveAbility DeactivatePassiveAbility;

	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>> Abilities, bool ActivateImmediately = false);
	void RemoveCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>> Abilities);

	bool bStartupAbilitiesGiven = false;
	
	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	void ForEachAbility(const FForEachAbility& Delegate);

	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
protected:

	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);

	virtual void OnRep_ActivateAbilities() override;

private:

	TMap<FGameplayTag,  FGameplayAbilitySpecHandle> OwnedAbilities;
};
