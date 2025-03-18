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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag DebuffTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DebuffDuration = 3.f;

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DebuffFrequency = 0.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DebuffChance = 100.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DebuffDamage = 0.f;
};

USTRUCT(BlueprintType)
struct FDamageEffectParams
{
	GENERATED_BODY()

	FDamageEffectParams(){}

	UPROPERTY()
	TObjectPtr<UObject> WorldContextObject = nullptr;

	UPROPERTY()
	TSubclassOf<UGameplayEffect> DamageGameplayEffectClass = nullptr;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> SourceAbilitySystemComponent = nullptr;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> TargetAbilitySystemComponent = nullptr;

	UPROPERTY()
	float BaseDamage = 0.0f;
	UPROPERTY()
	float AbilityLevel = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FDebuffInfo> DebuffTags;

	UPROPERTY()
	float DeathImpulseMagnitude = 0.f;
	
	UPROPERTY()
	FVector DeathImpulse = FVector::ZeroVector;
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

	FDamageEffectParams MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor = nullptr) const;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	TArray<FDebuffInfo> DebuffTags;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	FScalableFloat Damage;


	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float DeathImpulseMagnitude = 60.0f;
	
	UFUNCTION(BlueprintPure)
	FTaggedMontage GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontages) const;
};
