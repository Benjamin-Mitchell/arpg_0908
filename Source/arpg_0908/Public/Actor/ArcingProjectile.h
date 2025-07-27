// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "Actor/ArpgProjectile.h"
#include "ArcingProjectile.generated.h"

USTRUCT(BlueprintType)
struct FArcingProjectileInitData
{
	GENERATED_BODY()
    
	UPROPERTY()
	FVector ArcTargetLocation = FVector::ZeroVector;
    
	UPROPERTY()
	float ProjectileSpeed = 550.f;
};
/**
 * 
 */
UCLASS()
class ARPG_0908_API AArcingProjectile : public AArpgProjectile
{
	GENERATED_BODY()
public:
	AArcingProjectile();

	UPROPERTY(BlueprintReadOnly)
	FVector ArcTargetLocation;
	
	// Replicated initialization data
	UPROPERTY(ReplicatedUsing=OnRep_InitializationData)
	FArcingProjectileInitData InitData;
	
protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	void OnRep_InitializationData();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
private:
	float Alpha = 0;
	float Distance = 0;
	float TimeToTarget = 0;
	FVector SourceLocation;
	
	void Initialize();
};

