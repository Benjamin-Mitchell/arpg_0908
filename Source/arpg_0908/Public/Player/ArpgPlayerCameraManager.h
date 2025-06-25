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
	AArpgPlayerCameraManager();

	UFUNCTION(BlueprintCallable, Category="ArpgPlayerCameraManager")
	void SetFrozen();

	void SetTransitionTowardsFrozenTarget(FVector InTargetLocation, FRotator InTargetRotator, float InTransitionDuration, bool InTransitionRotation, bool InTransitionZValue);
	
	UFUNCTION(BlueprintCallable, Category="ArpgPlayerCameraManager")
	void SetTransitionViaOffset(FVector TransitionOffset, FRotator InTargetRotator, float InTransitionDuration, bool InTransitionRotation);

	UFUNCTION(BlueprintCallable, Category="ArpgPlayerCameraManager")
	void SetReturnToGameplay();

	FVector GetLastKnownLocation();
	
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

	//TODO: Make this replicated?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ArpgPlayerCameraManager")
	ECameraState CurrentCameraState = ECameraState::Gameplay;
};
