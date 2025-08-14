// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "ArpgHomingProjectileMovement.generated.h"

/**
 * 
 */
UCLASS()
class ARPG_0908_API UArpgHomingProjectileMovement : public UProjectileMovementComponent
{
	GENERATED_BODY()
	
public:
	// Forward acceleration parameters
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Acceleration")
	float AccelerationRate = 200.f; // units/sec²

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Acceleration")
	float MaxForwardSpeed = 1500.f;

	// Homing control
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Homing")
	float MinHomingAcceleration = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Homing")
	float MaxHomingAcceleration = 3000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Homing")
	float SnapDistance = 50.f;

protected:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	float CurrentForwardSpeed = 0.f;
};
