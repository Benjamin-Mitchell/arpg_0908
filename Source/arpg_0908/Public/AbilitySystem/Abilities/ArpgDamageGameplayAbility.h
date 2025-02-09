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

	float DebuffDuration = 3.f;
	float DebuffFrequency = 0.f;
	float DebuffChance = 100.f;
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

	UPROPERTY()
	TMap<FGameplayTag, FDebuffInfo> DebuffTags;	
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
	TMap<FGameplayTag, FDebuffInfo> DebuffTags;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	FScalableFloat Damage;
	
	
	UFUNCTION(BlueprintPure)
	FTaggedMontage GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontages) const;
};
