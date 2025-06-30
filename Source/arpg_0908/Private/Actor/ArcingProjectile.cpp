// Copyright Tinzie Games


#include "Actor/ArcingProjectile.h"

#include "Net/UnrealNetwork.h"

AArcingProjectile::AArcingProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
}
void AArcingProjectile::Initialize()
{
	ArcTargetLocation = InitData.ArcTargetLocation;
	ProjectileSpeed = InitData.ProjectileSpeed;
	
	SourceLocation = GetActorLocation();
	Alpha = 0;
	
	Distance = (ArcTargetLocation - SourceLocation).Length();
	TimeToTarget = Distance / ProjectileSpeed;
}

void AArcingProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	if (!HasAuthority()) return;
	
	Initialize();
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

	float HeightModulation = (1.0f - Beta * Beta);
	FVector HeightOffset = Distance * HeightModulation  * FVector(0.0, 0.0, 1.0);

	SetActorLocation(SourceLocation + (ArcTargetLocation - SourceLocation) * Alpha + HeightOffset);
}

void AArcingProjectile::OnRep_InitializationData()
{
	Initialize();
}

void AArcingProjectile::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AArcingProjectile, InitData);
}
