// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "ArpgPlayerCameraManager.generated.h"

UENUM(BlueprintType)
enum class ECameraState : uint8
{
	Frozen,
	TransitionTowardsFrozenTarget,
	Gameplay
};


/**
 * 
 */
UCLASS()
class ARPG_0908_API AArpgPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="ArpgPlayerCameraManager")
	void SetFrozen();

	UFUNCTION(BlueprintCallable, Category="ArpgPlayerCameraManager")
	void SetTransitionTowardsFrozenTarget(FVector InTargetLocation, FRotator InTargetRotator, float InTransitionDuration, bool InTransitionRotation, bool InTransitionZValue);

	UFUNCTION(BlueprintCallable, Category="ArpgPlayerCameraManager")
	void SetTransitionViaOffset(FVector TransitionOffset, FRotator InTargetRotator, float InTransitionDuration, bool InTransitionRotation);

	UFUNCTION(BlueprintCallable, Category="ArpgPlayerCameraManager")
	void SetReturnToGameplay();
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ArpgPlayerCameraManager")
	FVector TransitionTargetLocation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ArpgPlayerCameraManager")
	FRotator TransitionTargetRotator;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ArpgPlayerCameraManager")
	float CurrentTransitionTotalDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ArpgPlayerCameraManager")
	float CurrentTransitionDuration = 0.0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ArpgPlayerCameraManager")
	FTransform LastKnownCameraTransform;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ArpgPlayerCameraManager")
	ECameraState CurrentCameraState = ECameraState::Gameplay;
	

};
