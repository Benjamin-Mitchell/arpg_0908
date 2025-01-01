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
	FGameplayTag Damage_Basic;
	FGameplayTag Damage_Fire;

	//The game supports damage types, althogh resistances are not currently implemented. Can be implemented if required later.
	TArray<FGameplayTag> DamageTypes;
	
	FGameplayTag Abilities_Attack;

	
	FGameplayTag Abilities_Firebolt;
	FGameplayTag Abilities_Hook;
	FGameplayTag Abilities_HatProjectileTemp;
	FGameplayTag Abilities_DodgeRoll;

	FGameplayTag Montage_Attack_Weapon;
	FGameplayTag Montage_Attack_RightHand;
	FGameplayTag Montage_Attack_LeftHand;

	FGameplayTag Effects_HitReact;
	FGameplayTag Effects_HookPull;
	FGameplayTag Effects_DodgeRoll;
	
protected:
 
private:
    static FArpgGameplayTags GameplayTags;
};