// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/arpgGameplayAbility.h"
#include "Interaction/CombatInterface.h"
#include "ArpgDamageGameplayAbility.generated.h"


USTRUCT(BlueprintType)
struct FDebuffInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag DebuffTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DebuffDuration = 3.f;

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DebuffFrequency = 0.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DebuffChance = 100.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DebuffDamage = 0.f;
};

USTRUCT(BlueprintType)
struct FDamageEffectParams
{
	GENERATED_BODY()

	FDamageEffectParams(){}

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UObject> WorldContextObject = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> DamageGameplayEffectClass = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> SourceAbilitySystemComponent = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> TargetAbilitySystemComponent = nullptr;

	UPROPERTY(BlueprintReadWrite)
	float BaseDamage = 0.0f;
	UPROPERTY(BlueprintReadWrite)
	float AbilityLevel = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FDebuffInfo> DebuffTags;

	UPROPERTY(BlueprintReadWrite)
	float DeathImpulseMagnitude = 0.f;
	
	UPROPERTY(BlueprintReadWrite)
	FVector DeathImpulse = FVector::ZeroVector;
		
	UPROPERTY(BlueprintReadWrite)
	float KnockbackForceMagnitude = 3000.f;

	UPROPERTY(BlueprintReadWrite)
	FVector KnockbackForce = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite)
	float KnockbackChance = 0.f;
	
	UPROPERTY(BlueprintReadWrite)
	float KnockbackPitch = 45.f;
};
/**`
 * 
 */
UCLASS()
class ARPG_0908_API UArpgDamageGameplayAbility : public UarpgGameplayAbility
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* TargetActor);

	UFUNCTION(BlueprintPure)
	FDamageEffectParams MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor = nullptr) const;
	
	UFUNCTION(BlueprintPure)
	float GetDamageAtLevel() const;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	TArray<FDebuffInfo> DebuffTags;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	FScalableFloat Damage;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float DeathImpulseMagnitude = 6000.0f;
	
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float KnockbackForceMagnitude = 3000.f;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float KnockbackChance = 0.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float KnockbackPitch = 45.f;
	
	UFUNCTION(BlueprintPure)
	FTaggedMontage GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontages) const;

	UPROPERTY(BlueprintReadWrite)
	FVector TargetLocation;
	
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float TraversalDistanceMin = 0.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float TraversalDistanceMax = 0.f;

	UFUNCTION(BlueprintCallable)
	void SetTargetsIfTargetExists(bool SnapToFloorBelowTarget, AActor* TargetActor);

};
