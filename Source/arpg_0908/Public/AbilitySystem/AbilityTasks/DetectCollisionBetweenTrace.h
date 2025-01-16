// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "DetectCollisionBetweenTrace.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCollisionFoundEventSignature, const FGameplayAbilityTargetDataHandle&,  DataHandle);
/**
 * 
 */
UCLASS()
class ARPG_0908_API UDetectCollisionBetweenTrace : public UAbilityTask
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "DetectCollisionBetweenTrace", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = true))
	static UDetectCollisionBetweenTrace* CreateDetectCollisionBetweenTrace(USkeletalMeshComponent* InSkeletalMesh, int InIntermediateTraceCount, bool InShowDebugLines, UGameplayAbility* OwningAbility);

	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;

	//virtual void OnDestroy(bool AbilityIsEnding) override;

	UPROPERTY(BlueprintAssignable)
	FCollisionFoundEventSignature OnCollisionFound;

protected:

	//Object Pointer to a Skeletal Mesh. Not using TObjectPtr because UFUNCTION Can't take it as parameter.
	USkeletalMeshComponent* SkeletalMesh;

	int IntermediateTraceCount;
	bool FirstRay;
	bool ShowDebugLines;
	
	FVector PreviousStartLocation, PreviousEndLocation;
	TArray<uint32> UniqueObjectIDsHit;
	
}; 
