// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "Actor/ArpgProjectile.h"
#include "HookProjectile.generated.h"

/**
 * 
 */
UCLASS()
class ARPG_0908_API AHookProjectile : public AArpgProjectile
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "HookProjectile")
	FTransform GetHookTransform() {return HookStartTransform;};

	void SetHookTransform(FTransform Transform) {HookStartTransform = Transform;};

protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	
private:
	FTransform HookStartTransform;

	
	
	
};
