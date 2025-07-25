// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "arpgAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /*Asset Tags*/)

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAbilityChange, UarpgAbilitySystemComponent* )
DECLARE_MULTICAST_DELEGATE_OneParam(FDeactivatePassiveAbility, const FGameplayTag& /*Ablity Tag*/)
DECLARE_DELEGATE_TwoParams(FForEachAbility, const FGameplayAbilitySpec&, bool /*MarkedForDeletion*/)

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityActivateFailed, const FGameplayTag&, FailedInputTag);

USTRUCT()
struct FOwnedAbilityHandle
{
	GENERATED_BODY()

	FGameplayAbilitySpec Spec;
	FGameplayAbilitySpecHandle Handle;

};
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
	FOnAbilityChange AbilitiesChanged;
	FDeactivatePassiveAbility DeactivatePassiveAbility;

	UPROPERTY(BlueprintAssignable, Category="GAS|Messages")
	FAbilityActivateFailed AbilityActivateFailed;

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

	bool bReleasedPress = false;
	TMap<FGameplayTag,  FOwnedAbilityHandle> OwnedAbilities;
	TArray<FGameplayAbilitySpec> DeletedAbilitySpecs;
};
