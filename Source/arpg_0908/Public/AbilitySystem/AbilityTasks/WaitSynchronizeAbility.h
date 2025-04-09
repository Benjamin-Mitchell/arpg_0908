// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "WaitSynchronizeAbility.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNewCasterDelegate, const FGameplayAbilityTargetDataHandle&,  DataHandle);
/**
 * 
 */
UCLASS()
class ARPG_0908_API UWaitSynchronizeAbility : public UAbilityTask
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "WaitSynchronizeAbility", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = true))
	static UWaitSynchronizeAbility* WaitForCheckPlayersWithTag(
		UGameplayAbility* OwningAbility,
		APawn* OwningActor,
		FGameplayTag RequiredTag,
		float Interval = 0.1f // How often to check
	);

	UPROPERTY(BlueprintAssignable)
	FNewCasterDelegate OnNewCaster;

	
	UFUNCTION(BlueprintCallable)
	TArray<AActor*> GetOtherActorsWithSynchronizeTag();

	void Check();


	virtual void Activate() override;
	virtual void OnDestroy(bool AbilityEnded) override;
	
private:
	FGameplayTag SynchronizeTag;
	float CheckInterval;
	APawn* OwningActor;
	FTimerHandle TimerHandle;

	TArray<AActor*> KnownCasters;
	int32 OwningPlayerIndex;

	void BroadCastCasters();
};
