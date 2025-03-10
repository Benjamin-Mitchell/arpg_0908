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
	FGameplayTag InputTag_Interact;
	FGameplayTag InputTag_Dodge;

	FGameplayTag Damage;

	//Debuff types
	FGameplayTag Debuff_Burn;
	FGameplayTag Debuff_Stun;

	//Debuff parameters
	FGameplayTag Debuff_Duration;
	FGameplayTag Debuff_Frequency;
	FGameplayTag Debuff_Chance;
	FGameplayTag Debuff_Damage;

	TArray<FGameplayTag> DebuffTags;

	
	FGameplayTag Abilities_Attack;

	
	FGameplayTag Abilities_Firebolt;
	FGameplayTag Abilities_Hook;
	FGameplayTag Abilities_HatProjectileTemp;
	FGameplayTag Abilities_DodgeRoll;

	FGameplayTag Montage_Attack_Weapon;
	FGameplayTag Montage_Attack_RightHand;
	FGameplayTag Montage_Attack_LeftHand;

	//Line-Trace Tags
	FGameplayTag Montage_Attack_DamageTraceBegin;
	FGameplayTag Montage_Attack_DamageTraceEnd;

	//Specials that need more attention
	FGameplayTag Montage_Attack_CustomOrDebug;

	FGameplayTag Effects_HitReact;
	FGameplayTag Effects_HookPull;
	FGameplayTag Effects_DodgeRoll;
	
protected:
 
private:
    static FArpgGameplayTags GameplayTags;
};