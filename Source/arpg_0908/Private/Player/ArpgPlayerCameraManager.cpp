// Copyright Tinzie Games


#include "Player/ArpgPlayerCameraManager.h"

void AArpgPlayerCameraManager::SetFrozen()
{
	CurrentCameraState = ECameraState::Frozen;
}

void AArpgPlayerCameraManager::SetTransitionTowardsFrozenTarget(FVector InTargetLocation, FRotator InTargetRotator, float InTransitionDuration, bool InTransitionRotation, bool InTransitionZValue)
{
	CurrentCameraState = ECameraState::TransitionTowardsFrozenTarget;

	if (InTransitionZValue)
		TransitionTargetLocation = InTargetLocation;
	else
	{
		TransitionTargetLocation.X = InTargetLocation.X;
		TransitionTargetLocation.Y = InTargetLocation.Y;
		TransitionTargetLocation.Z = LastKnownCameraTransform.GetLocation().Z;
	}
	
	CurrentTransitionTotalDuration = InTransitionDuration;
	CurrentTransitionDuration = 0.0;

	if (InTransitionRotation)
		TransitionTargetRotator = InTargetRotator;
	else
		TransitionTargetRotator = LastKnownCameraTransform.Rotator();
}

void AArpgPlayerCameraManager::SetTransitionViaOffset(FVector TransitionOffset, FRotator InTargetRotator,
	float InTransitionDuration, bool InTransitionRotation)
{
	FVector LocalNewTarget = LastKnownCameraTransform.GetLocation() + TransitionOffset;
	SetTransitionTowardsFrozenTarget(LocalNewTarget, InTargetRotator, InTransitionDuration, InTransitionRotation, true);
}

void AArpgPlayerCameraManager::SetReturnToGameplay()
{
	CurrentCameraState = ECameraState::Gameplay;
}


