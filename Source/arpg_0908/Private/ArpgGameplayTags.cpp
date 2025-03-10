// Copyright Tinzie Games


#include "ArpgGameplayTags.h"
#include "GameplayTagsManager.h"

FArpgGameplayTags FArpgGameplayTags::GameplayTags;

void FArpgGameplayTags::InitializeNativeGameplayTags()
{
	GameplayTags.Attributes_Power_AttackDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Power.AttackDamage"));
	GameplayTags.Attributes_Power_AbilityPower = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Power.AbilityPower"));
	
	GameplayTags.Attributes_Defense_DamageReduction = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Defense.DamageReduction"));
	
	GameplayTags.Attributes_Miscellaneous_MoveSpeed = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Miscellaneous.MoveSpeed"));
	GameplayTags.Attributes_Miscellaneous_Critchance = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Miscellaneous.CritChance"));
	GameplayTags.Attributes_Miscellaneous_CritDamageMultiplier = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Miscellaneous.CritDamageMultiplier"));
	GameplayTags.Attributes_Miscellaneous_NumberOfDashes = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Miscellaneous.NumberOfDashes"));
	
	GameplayTags.Attributes_Miscellaneous_MaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Miscellaneous.MaxHealth"));
	GameplayTags.Attributes_Miscellaneous_MaxMana = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Miscellaneous.MaxMana"));
	
	GameplayTags.Attributes_Vital_Health = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Vital.Health"));
	GameplayTags.Attributes_Vital_Mana = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Vital.Mana"));

	GameplayTags.Attributes_Primary_Strength = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Primary.Strength"));
	GameplayTags.Attributes_Primary_Intelligence = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Primary.Intelligence"));
	GameplayTags.Attributes_Primary_Vigor = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Primary.Vigor"));


	GameplayTags.InputTag_LMB = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.LMB"));
	GameplayTags.InputTag_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.RMB"));
	GameplayTags.InputTag_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.1"));
	GameplayTags.InputTag_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.2"));
	GameplayTags.InputTag_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.3"));
	GameplayTags.InputTag_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.4"));
	GameplayTags.InputTag_Interact = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Interact"));
	GameplayTags.InputTag_Dodge = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Dodge"));
	
	GameplayTags.Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Damage"),
		FString("Tag for assigning magnitude of ability specs (amount of damage to do)"));

	GameplayTags.Debuff_Burn = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Debuff.Burn"));
	GameplayTags.DebuffTags.Add(GameplayTags.Debuff_Burn);
	GameplayTags.Debuff_Stun = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Debuff.Stun"));
	GameplayTags.DebuffTags.Add(GameplayTags.Debuff_Stun);

	GameplayTags.Debuff_Duration = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Debuff.Duration"));
	GameplayTags.Debuff_Frequency = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Debuff.Frequency"));
	GameplayTags.Debuff_Chance = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Debuff.Chance"));
	GameplayTags.Debuff_Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Debuff.Damage"));
	

	GameplayTags.Abilities_Attack = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.Attack"));
	
	GameplayTags.Abilities_Firebolt = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.Firebolt"));
	GameplayTags.Abilities_Hook = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.Hook"));
	GameplayTags.Abilities_HatProjectileTemp = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.HatProjectileTemp"));
	GameplayTags.Abilities_DodgeRoll = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.DodgeRoll"));

	GameplayTags.Effects_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Effects.HitReact"));
	GameplayTags.Effects_HookPull = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Effects.HookPull"));
	GameplayTags.Effects_DodgeRoll = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Effects.DodgeRoll"));


	GameplayTags.Montage_Attack_Weapon = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Montage.Attack.Weapon"));
	GameplayTags.Montage_Attack_RightHand = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Montage.Attack.RightHand"));
	GameplayTags.Montage_Attack_LeftHand = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Montage.Attack.LeftHand"));
	GameplayTags.Montage_Attack_DamageTraceBegin = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Montage.Attack.DamageTraceBegin"));
	GameplayTags.Montage_Attack_DamageTraceEnd = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Montage.Attack.DamageTraceEnd"));
	GameplayTags.Montage_Attack_CustomOrDebug = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Montage.Attack.CustomOrDebug"));
	
	
}
