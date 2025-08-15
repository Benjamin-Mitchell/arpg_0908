// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "Actor/Projectiles/ArpgProjectile.h"
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

	//Adding an important comment
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Homing")
	bool bDestroyOnReachHomingTargetExact = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Homing")
	float DistanceFromTargetToDestroy = 5.0;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	UFUNCTION(BlueprintCallable)
	void SetHoming(USceneComponent* HomingTarget, bool IsHoming = true);
	
	virtual void Fire() override;
};
