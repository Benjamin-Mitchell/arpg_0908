// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "arpgGameplayAbility.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGameplayAbilityEnded);
/**
 * 
 */
UCLASS()
class ARPG_0908_API UarpgGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Input")
	FGameplayTag StartupInputTag;

	FGameplayAbilityEnded OnAbilityEnded;

	bool GetEndedAlready() const;
protected:
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
private:
	bool EndedAlready = false;
};
