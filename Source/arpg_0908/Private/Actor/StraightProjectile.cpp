// Copyright Tinzie Games


#include "Actor/StraightProjectile.h"

#include "Projects.h"
#include "GameFramework/ProjectileMovementComponent.h"

AStraightProjectile::AStraightProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->InitialSpeed = ProjectileSpeed;
	ProjectileMovement->MaxSpeed = ProjectileSpeed;
	ProjectileMovement->ProjectileGravityScale = 0.f;

	//replicate for if we want to set e.g. homing
	ProjectileMovement->SetIsReplicated(true);

	if (!bFireImmediatelyOnSpawn)
		ProjectileMovement->Deactivate();
}

void AStraightProjectile::SetHoming(USceneComponent* HomingTarget, bool IsHoming)
{
	ProjectileMovement->bIsHomingProjectile = IsHoming;
	ProjectileMovement->HomingTargetComponent = HomingTarget;
}

void AStraightProjectile::Fire()
{
	ProjectileMovement->Activate();
}
