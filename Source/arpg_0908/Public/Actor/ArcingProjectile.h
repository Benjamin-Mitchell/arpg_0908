// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "Actor/ArpgProjectile.h"
#include "ArcingProjectile.generated.h"

/**
 * 
 */
UCLASS()
class ARPG_0908_API AArcingProjectile : public AArpgProjectile
{
	GENERATED_BODY()
public:
	AArcingProjectile();

	
	FVector ArcTargetLocation;
protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

private:
	float Alpha = 0;
	float Distance = 0;
	float TimeToTarget = 0;
	FVector SourceLocation;
	
	
};
