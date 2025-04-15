// Copyright Tinzie Games


#include "Actor/StraightProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"

AStraightProjectile::AStraightProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->InitialSpeed = ProjectileSpeed;
	ProjectileMovement->MaxSpeed = ProjectileSpeed;
	ProjectileMovement->ProjectileGravityScale = 0.f;
}
