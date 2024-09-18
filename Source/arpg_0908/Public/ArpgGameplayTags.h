// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 * ArpgGameplayTags
 * Singletion Containing Native Gameplay Tags
 */
struct FArpgGameplayTags
{
public:
    static const FArpgGameplayTags& Get() {return GameplayTags;};
    static void InitializeNativeGameplayTags();

    FGameplayTag Attributes_Power_AttackDamage;
    FGameplayTag Attributes_Power_AbilityPower;
	
    FGameplayTag Attributes_Defense_DamageReduction;

	FGameplayTag Attributes_Miscellaneous_MoveSpeed;
	FGameplayTag Attributes_Miscellaneous_Critchance;
	FGameplayTag Attributes_Miscellaneous_CritDamageMultiplier;
	FGameplayTag Attributes_Miscellaneous_NumberOfDashes;

	FGameplayTag Attributes_Miscellaneous_MaxHealth;
	FGameplayTag Attributes_Miscellaneous_MaxMana;
	
	FGameplayTag Attributes_Vital_Health;
	FGameplayTag Attributes_Vital_Mana;
	
	FGameplayTag Attributes_Primary_Strength;
	FGameplayTag Attributes_Primary_Intelligence;
	FGameplayTag Attributes_Primary_Vigor;


	FGameplayTag InputTag_LMB;
	FGameplayTag InputTag_RMB;
	FGameplayTag InputTag_1;
	FGameplayTag InputTag_2;
	FGameplayTag InputTag_3;
	FGameplayTag InputTag_4;

	FGameplayTag Damage;

	FGameplayTag Effects_HitReact;
	
protected:
 
private:
    static FArpgGameplayTags GameplayTags;
};