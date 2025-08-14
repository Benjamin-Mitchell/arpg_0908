// Copyright Tinzie Games


#include "Actor/Projectiles/ArpgHomingProjectileMovement.h"

void UArpgHomingProjectileMovement::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!UpdatedComponent)
		return;

	// Gradual forward acceleration
	CurrentForwardSpeed = FMath::Min(CurrentForwardSpeed + (AccelerationRate * DeltaTime), MaxForwardSpeed);
	Velocity = Velocity.GetSafeNormal() * CurrentForwardSpeed;

	// Dynamic homing adjustment
	if (HomingTargetComponent.IsValid())
	{
		const FVector ToTarget = HomingTargetComponent->GetComponentLocation() - UpdatedComponent->GetComponentLocation();
		const float Distance = ToTarget.Size();

		if (Distance < SnapDistance)
			Velocity = ToTarget.GetSafeNormal() * CurrentForwardSpeed;
	

		float Alpha = CurrentForwardSpeed / MaxForwardSpeed;
		
		HomingAccelerationMagnitude = FMath::Lerp(MinHomingAcceleration, MaxHomingAcceleration, Alpha);
	}
}
