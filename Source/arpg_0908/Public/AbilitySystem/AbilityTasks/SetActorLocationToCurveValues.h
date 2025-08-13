// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "SetActorLocationToCurveValues.generated.h"

/**
 * 
 */
UCLASS()
class ARPG_0908_API USetActorLocationToCurveValues : public UAbilityTask
{
	GENERATED_BODY()

public:	
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta = (DisplayName="Move Actor Along Montage Curve", HidePin="OwningAbility", DefaultToSelf="OwningAbility"))
	static USetActorLocationToCurveValues* MoveActorAlongMontageCurve(
		UGameplayAbility* OwningAbility,
		AActor* TargetActor,
		FName CurveNameX,
		FName CurveNameY,
		FName CurveNameZ,
		FVector BaseLocation,
		bool bUseActorStartAsBase = false
	);

	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;
	// virtual void OnDestroy(bool bInOwnerFinished) override;


private:
	AActor* ActorToMove;

	FName CurveX;
	FName CurveY;
	FName CurveZ;

	FVector BaseLoc;
	bool bBaseFromActor;

	FVector InitialActorLoc;
};
