// Copyright Tinzie Games

#pragma once


#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "ListenForAbilityEndedByTag.generated.h"

struct FGameplayTagContainer;
class UarpgGameplayAbility;
class UAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGameplayAbilityCaughtByTag);
/**
 * 
 */
UCLASS()
class ARPG_0908_API UListenForAbilityEndedByTag : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FGameplayAbilityCaughtByTag OnAbilityEnd;
	
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UListenForAbilityEndedByTag* ListenForAbilityEndedByTag(UAbilitySystemComponent* AbilitySystemComponent, FGameplayTagContainer Tag);

	UFUNCTION()
	void OnAbilityEndCallback();

	UFUNCTION(BlueprintCallable)
	void EndTask();
	
protected:
	UarpgGameplayAbility* AbilityListeningTo;
};
