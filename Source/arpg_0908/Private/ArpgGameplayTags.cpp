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
	
	GameplayTags.Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Damage"));
	
}
