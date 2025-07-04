// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/ArpgDamageGameplayAbility.h"
#include "GameFramework/Actor.h"
#include "ArpgProjectile.generated.h"

class UNiagaraSystem;
class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class ARPG_0908_API AArpgProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AArpgProjectile();



	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	FDamageEffectParams DamageEffectParams;

	float ProjectileSpeed = 550.f;

	bool CollisionEnabled = true;

	UFUNCTION(BlueprintCallable, Category = "HookProjectile")
	FTransform GetSpawnTransform() {return SpawnTransform;};

	void SetSpawnTransform(FTransform Transform) {SpawnTransform = Transform;};

	void SetCollisionTags(const FGameplayTagContainer& InWithTags, const FGameplayTagContainer& InPassThroughTags,
	const TArray<AActor*> &InAvoidActors);
protected:
	virtual void BeginPlay() override;
	void OnHit();
	virtual void Destroyed() override;

	UFUNCTION(BlueprintImplementableEvent)
	void CustomOnDestroyLogic(AActor* OtherActor);

	UFUNCTION(BlueprintCallable)
	void DamageTargetASC(AActor* OtherActor);

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	bool IsValidOverlap(AActor* OtherActor, bool& OutOtherActorHasASC);

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(EditDefaultsOnly)
	float LifeSpan = 15.f;
	
	
	
	bool bHit = false;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> ImpactEffect;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> ImpactSound;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> LoopingSound;

	UPROPERTY()
	TObjectPtr<UAudioComponent> LoopingSoundComponent;
	
	FTransform SpawnTransform;

	UPROPERTY(Replicated)
	FGameplayTagContainer WithTags;

	UPROPERTY(Replicated)
	FGameplayTagContainer PassThroughTags;

	UPROPERTY(Replicated)
	TArray<TObjectPtr<AActor>> AvoidActors;

};
