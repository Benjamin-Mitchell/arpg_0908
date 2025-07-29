// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "ListenForGameplayAbilityEnded.generated.h"

class UGameplayAbility;
class UarpgGameplayAbility;
class UAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGameplayAbilityCaught);
/**
 * 
 */
UCLASS(BlueprintType, meta = (ExposedAsyncProxy = "AsyncTask"))
class ARPG_0908_API UListenForGameplayAbilityEnded : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FGameplayAbilityCaught OnAbilityEnd;
	
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UListenForGameplayAbilityEnded* ListenForGameplayAbilityEnded(UAbilitySystemComponent* AbilitySystemComponent, TSubclassOf<UGameplayAbility> abilityClass);

	UFUNCTION()
	void OnAbilityEndCallback();

	UFUNCTION(BlueprintCallable)
	void EndTask();
	
protected:
	UarpgGameplayAbility* AbilityListeningTo;
};
