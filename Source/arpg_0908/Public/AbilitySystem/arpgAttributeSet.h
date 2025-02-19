// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "arpgAttributeSet.generated.h"
 
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
 GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
 GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
 GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
 GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

USTRUCT()
struct FEffectProperties
{
	GENERATED_BODY()

	FEffectProperties(){}

	FGameplayEffectContextHandle EffectContextHandle;

	UPROPERTY()
	UAbilitySystemComponent* SourceASC = nullptr;

	UPROPERTY()
	AActor* SourceAvatarActor = nullptr;

	UPROPERTY()
	AController* SourceController = nullptr;

	UPROPERTY()
	ACharacter* SourceCharacter = nullptr;


	UPROPERTY()
	UAbilitySystemComponent* TargetASC = nullptr;

	UPROPERTY()
	AActor* TargetAvatarActor = nullptr;

	UPROPERTY()
	AController* TargetController = nullptr;

	UPROPERTY()
	ACharacter* TargetCharacter = nullptr;
	
};

/**
 * 
 */
UCLASS()
class ARPG_0908_API UarpgAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UarpgAttributeSet();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;



	//Basic Power Stats
	//Attack Damage
	//Ability Power
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AttackDamage, Category = "Power Attributes")
	FGameplayAttributeData AttackDamage;
	ATTRIBUTE_ACCESSORS(UarpgAttributeSet, AttackDamage);
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AbilityPower, Category = "Power Attributes")
	FGameplayAttributeData AbilityPower;
	ATTRIBUTE_ACCESSORS(UarpgAttributeSet, AbilityPower);
	

	//Basic Defense Stats
	//Damage Reduction (Percent)	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_DamageReduction, Category = "Defense Attributes")
	FGameplayAttributeData DamageReduction;
	ATTRIBUTE_ACCESSORS(UarpgAttributeSet, DamageReduction);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BlockChance, Category = "Defense Attributes")
	FGameplayAttributeData BlockChance;
	ATTRIBUTE_ACCESSORS(UarpgAttributeSet, BlockChance);

	//miscellaneous
	//Move Speed Multiplier (Percent)
	//Crit Chance Multiplier (Percent)
	//Crit Damage Multiplier (Percent)
	//Number of Dashes (Flat)
	//MaxHealth
	//MaxMana (TBD)
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MoveSpeed, Category = "Miscellaneous Attributes")
	FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS(UarpgAttributeSet, MoveSpeed);
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CritChance, Category = "Miscellaneous Attributes")
	FGameplayAttributeData CritChance;
	ATTRIBUTE_ACCESSORS(UarpgAttributeSet, CritChance);
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CritDamageMultiplier, Category = "Miscellaneous Attributes")
	FGameplayAttributeData CritDamageMultiplier;
	ATTRIBUTE_ACCESSORS(UarpgAttributeSet, CritDamageMultiplier);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_NumberOfDashes, Category = "Miscellaneous Attributes")
	FGameplayAttributeData NumberOfDashes;
	ATTRIBUTE_ACCESSORS(UarpgAttributeSet, NumberOfDashes);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Miscellaneous Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UarpgAttributeSet, MaxHealth);

	//MaxMana is left-over from Tutorial.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category = "Miscellaneous Attributes")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UarpgAttributeSet, MaxMana);
	


	
	//Vital Attributes
	//Current Health
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Vital Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UarpgAttributeSet, Health);

	//Mana is left-over from Tutorial.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "Vital Attributes")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UarpgAttributeSet, Mana);

	//Meta Attributes

	UPROPERTY(BlueprintReadOnly, Category = "Meta Attributes")
	FGameplayAttributeData IncomingDamage;
	ATTRIBUTE_ACCESSORS(UarpgAttributeSet, IncomingDamage);
	
	//Primary Attributes

	//Strength. TBD (from Tutorial)
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Strength, Category = "Primary Attributes")
	FGameplayAttributeData Strength;
	ATTRIBUTE_ACCESSORS(UarpgAttributeSet, Strength);

	//Intelligence. TBD (from Tutorial)
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Intelligence, Category = "Primary Attributes")
	FGameplayAttributeData Intelligence;
	ATTRIBUTE_ACCESSORS(UarpgAttributeSet, Intelligence);

	//Vigor. TBD (from Tutorial)
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Vigor, Category = "Primary Attributes")
	FGameplayAttributeData Vigor;
	ATTRIBUTE_ACCESSORS(UarpgAttributeSet, Vigor);
	
	//By default, using the value type itself as a parameter (the only option for parameter), the value will contain the old value.
	UFUNCTION()
	void OnRep_AttackDamage(const FGameplayAttributeData& OldAttackDamage) const;

	UFUNCTION()
	void OnRep_AbilityPower(const FGameplayAttributeData& OldAbilityPower) const;

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;
	
	UFUNCTION()
	void OnRep_DamageReduction(const FGameplayAttributeData& OldDamageReduction) const;
	
	UFUNCTION()
	void OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const;

	UFUNCTION()
	void OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed) const;
	
	UFUNCTION()
	void OnRep_CritChance(const FGameplayAttributeData& OldCritChance) const;
	
	UFUNCTION()
	void OnRep_CritDamageMultiplier(const FGameplayAttributeData& OldCritDamageMultiplier) const;

	UFUNCTION()
	void OnRep_NumberOfDashes(const FGameplayAttributeData& OldNumberOfDashes) const;
	
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const;	

	
	//From Tutorial
	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldMana) const;

	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const;

	UFUNCTION()
	void OnRep_Strength (const FGameplayAttributeData& OldStrength) const;
	
	UFUNCTION()
	void OnRep_Intelligence (const FGameplayAttributeData& OldIntelligence) const;
	
	UFUNCTION()
	void OnRep_Vigor (const FGameplayAttributeData& OldVigor) const;

private:

	void HandleIncomingDamage(const FEffectProperties& EffectProps);

	void Debuff(const FEffectProperties& EffectProps);

	void SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const;

	void ShowFloatingText(const FEffectProperties& EffectProps, float Damage, bool bBlockedHit, bool bCriticalHit) const;
};
