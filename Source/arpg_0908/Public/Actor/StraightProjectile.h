// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "Actor/ArpgProjectile.h"
#include "StraightProjectile.generated.h"

/**
 * 
 */
UCLASS()
class ARPG_0908_API AStraightProjectile : public AArpgProjectile
{
	GENERATED_BODY()
public:
	AStraightProjectile();

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Homing")
	bool bDestroyOnReachHomingTargetExact = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Homing")
	float DistanceFromTargetToDestroy = 5.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Homing")
	float MinHomingAcceleration = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Homing")
	float MaxHomingAcceleration = 5000.f;

	// Distance range for scaling
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Homing")
	float HomingLerpMinDistance = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Homing")
	float HomingLerpMaxDistance = 1000.f;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	UFUNCTION(BlueprintCallable)
	void SetHoming(USceneComponent* HomingTarget, bool IsHoming = true);
	
	virtual void Fire() override;
};
