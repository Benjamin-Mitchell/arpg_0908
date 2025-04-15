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

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;
};
