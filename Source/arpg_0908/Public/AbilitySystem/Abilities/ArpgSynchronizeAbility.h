// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "ArpgDamageGameplayAbility.h"
#include "ArpgSynchronizeAbility.generated.h"

/**
 * 
 */
UCLASS()
class ARPG_0908_API UArpgSynchronizeAbility : public UArpgDamageGameplayAbility
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void RemoveSynchronizationTagToASC();

	UFUNCTION(BlueprintCallable)
	void AddSynchronizationTagToASC();

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag SynchronizeTag; 
};
