// Copyright Tinzie Games


#include "Actor/ArcingProjectile.h"

AArcingProjectile::AArcingProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AArcingProjectile::BeginPlay()
{
	Super::BeginPlay();

	SourceLocation = GetActorLocation();
	Alpha = 0;
	Distance = (ArcTargetLocation - SourceLocation).Length();
	TimeToTarget = Distance / ProjectileSpeed;
}

void AArcingProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Alpha = Alpha + (DeltaTime / TimeToTarget);

	if (Alpha > 1)
	{
		OnHit();
		OnDestroy(nullptr);
		Destroy();
	}

	float Beta = (Alpha - 0.5f) * 2.0f;
	
	FVector HeightOffset = Distance * (1.0f - Beta * Beta) * GetActorUpVector();

	SetActorLocation(SourceLocation + (ArcTargetLocation - SourceLocation) * Alpha + HeightOffset);
}
