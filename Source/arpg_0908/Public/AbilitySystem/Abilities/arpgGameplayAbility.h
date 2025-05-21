// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "arpgGameplayAbility.generated.h"

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

};
