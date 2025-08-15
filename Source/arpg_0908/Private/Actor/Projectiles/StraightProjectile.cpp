// Copyright Tinzie Games


#include "Actor/Projectiles/StraightProjectile.h"

#include "Projects.h"
#include "GameFramework/ProjectileMovementComponent.h"

AStraightProjectile::AStraightProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->InitialSpeed = ProjectileSpeed;
	ProjectileMovement->MaxSpeed = ProjectileSpeed;
	ProjectileMovement->ProjectileGravityScale = 0.f;

	//replicate for if we want to set e.g. homing
	ProjectileMovement->SetIsReplicated(true);

	if (!bFireImmediatelyOnSpawn)
		ProjectileMovement->Deactivate();
}

void AStraightProjectile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (ProjectileMovement->bIsHomingProjectile && ProjectileMovement->HomingTargetComponent.IsValid())
	{
		if (bDestroyOnReachHomingTargetExact)
		{
			float Dist = (ProjectileMovement->HomingTargetComponent->GetComponentLocation() - GetActorLocation()).Length();
			if (Dist < DistanceFromTargetToDestroy)
			{
				RegisterOnHit(true, ProjectileMovement->HomingTargetComponent->GetAttachmentRootActor());
			}
		}	
	}
	
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
