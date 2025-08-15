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

	UFUNCTION(BlueprintCallable, Category = "HookProjectile")
	FTransform GetSpawnTransform() {return SpawnTransform;};

	void SetSpawnTransform(FTransform Transform) {SpawnTransform = Transform;};

	void SetCollisionTags(const FGameplayTagContainer& InWithTags, const FGameplayTagContainer& InPassThroughTags,
	const TArray<AActor*> &InAvoidActors);

	UFUNCTION(BlueprintCallable)
	virtual void Fire(); //Call this only if bFireImmediatelyOnSpawn is false.
protected:
	virtual void BeginPlay() override;
	void OnHit();
	void RegisterOnHit(bool OtherActorHasASC, AActor* OtherActor);
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

	//This is functionality to not immediately enable the ProjectileMovementComponent (or other movement) on spawn.
	//"Fire" must be called at a later point. Mostly used for controlled-spawn sequences.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Properties")
	bool bFireImmediatelyOnSpawn = true; 
	
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
