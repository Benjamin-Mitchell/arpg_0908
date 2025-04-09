// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/arpgGameplayAbility.h"
#include "ArpgPassiveAbility.generated.h"

/**
 * 
 */
UCLASS()
class ARPG_0908_API UArpgPassiveAbility : public UarpgGameplayAbility
{
	GENERATED_BODY()
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	void ReceiveDeactivate(const FGameplayTag& AbilityTag);

protected:
	//Ability Duration, use a negative value to make it infinite.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	float AbilityDuration = -1.0f;

	UFUNCTION()
	void OnDurationFinished();
};
