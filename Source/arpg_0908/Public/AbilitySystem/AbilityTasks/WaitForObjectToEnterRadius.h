// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "WaitForObjectToEnterRadius.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFoundValidActorsDelegate, const FGameplayAbilityTargetDataHandle&,  DataHandle);

/**
 * 
 */
UCLASS()
class ARPG_0908_API UWaitForObjectToEnterRadius : public UAbilityTask
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta = (DisplayName="WaitForObjectToEnterRadius", HidePin="OwningAbility", DefaultToSelf="OwningAbility"))
	static UWaitForObjectToEnterRadius* CreateWaitForObjectToEnterRadius(
		UGameplayAbility* OwningAbility,
		const FGameplayTagContainer& InTargetTags,
		float InRadius,
		FVector InBaseLocation,
		float InWaitSecondsPerScan
	);
	
	UPROPERTY(BlueprintAssignable)
	FFoundValidActorsDelegate OnFoundActors;

	virtual void TickTask(float DeltaTime) override;
	virtual void Activate() override;
protected:
	FGameplayTagContainer InTargetTags;
	float Radius;
	float WaitSecondsPerScan;
	FVector BaseLocation;

	float ScanTimer;

	TWeakObjectPtr<AActor> Caster;

	TArray<AActor*> OverlappingActors;
	TArray<AActor*> ActorsToIgnore;
	TArray<FName> IgnoreTags;
	
};
