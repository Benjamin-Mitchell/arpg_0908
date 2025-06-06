// Copyright Tinzie Games


#include "ArpgGameplayTags.h"
#include "GameplayTagsManager.h"

FArpgGameplayTags FArpgGameplayTags::GameplayTags;

void FArpgGameplayTags::InitializeNativeGameplayTags()
{
	//Attribute Tags
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


	//Input Tags - used for tying abilities and UI elements to inputs.
	GameplayTags.InputTag_LMB = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.LMB"));
	GameplayTags.InputTag_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.RMB"));
	GameplayTags.InputTag_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.1"));
	GameplayTags.InputTag_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.2"));
	GameplayTags.InputTag_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.3"));
	GameplayTags.InputTag_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.4"));
	GameplayTags.InputTag_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.5"));
	GameplayTags.InputTag_Interact = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Interact"));
	GameplayTags.InputTag_Dodge = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Dodge"));
	GameplayTags.InputTag_Dance = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Dance"));
	
	GameplayTags.Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Damage"),
		FString("Tag for assigning magnitude of ability specs (amount of damage to do)"));

	//Debuff Tags - used for passing debuffs across the network to notify clients when an ability has a debuff
	GameplayTags.Debuff_Burn = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Debuff.Burn"));
	GameplayTags.DebuffTags.Add(GameplayTags.Debuff_Burn);
	GameplayTags.Debuff_Stun = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Debuff.Stun"));
	GameplayTags.DebuffTags.Add(GameplayTags.Debuff_Stun);

	GameplayTags.Debuff_Duration = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Debuff.Duration"));
	GameplayTags.Debuff_Frequency = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Debuff.Frequency"));
	GameplayTags.Debuff_Chance = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Debuff.Chance"));
	GameplayTags.Debuff_Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Debuff.Damage"));
	

	//Ability Tags - Used for tying gameplay and UI elements to equipped abilities
	GameplayTags.Abilities_Offensive_Attack = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.Offensive.Attack"));
	GameplayTags.Abilities_Offensive_Firebolt = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.Offensive.Firebolt"));
	GameplayTags.Abilities_Offensive_Electrocute = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.Offensive.Electrocute"));

	GameplayTags.Abilities_Passive_HaloOfProtection = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.Passive.HaloOfProtection"));
	GameplayTags.Abilities_Passive_LifeSiphon = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.Passive.LifeSiphon"));
	
	GameplayTags.Abilities_Synchronize_ElectrocuteWithFriends = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.Synchronize.ElectrocuteWithFriends"));
	GameplayTags.Abilities_Synchronize_InControl = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.Synchronize.InControl"));
	
	GameplayTags.Abilities_Hook = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.Hook"));
	GameplayTags.Abilities_HatProjectileTemp = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.HatProjectileTemp"));
	GameplayTags.Abilities_DodgeRoll = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.DodgeRoll"));

	//Cooldown tags
	GameplayTags.Cooldown_Offensive_BasicAttack = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Cooldown.Offensive.BasicAttack"));

	//Effect tags
	GameplayTags.Effects_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Effects.HitReact"));
	GameplayTags.Effects_HookPull = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Effects.HookPull"));
	GameplayTags.Effects_DodgeRoll = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Effects.DodgeRoll"));


	//Montage tags, used for signalling in montages when a specific attack is happening (one of the basic ones where damaged is applied around socket locations)
	GameplayTags.Montage_Attack_Weapon = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Montage.Attack.Weapon"));
	GameplayTags.Montage_Attack_RightHand = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Montage.Attack.RightHand"));
	GameplayTags.Montage_Attack_LeftHand = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Montage.Attack.LeftHand"));
	GameplayTags.Montage_Attack_RightFoot = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Montage.Attack.RightFoot"));
	GameplayTags.Montage_Attack_LeftFoot = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Montage.Attack.LeftFoot"));
	GameplayTags.Montage_Attack_UpperChest = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Montage.Attack.UpperChest"));
	GameplayTags.Montage_Attack_Head = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Montage.Attack.Head"));
	
	GameplayTags.Montage_Attack_DamageTraceBegin = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Montage.Attack.DamageTraceBegin"));
	GameplayTags.Montage_Attack_DamageTraceEnd = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Montage.Attack.DamageTraceEnd"));
	GameplayTags.Montage_Attack_CustomOrDebug = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Montage.Attack.CustomOrDebug"));

	//Block tags, used for preventing player actions during other actions (e.g. disable movement during stun)
	GameplayTags.Player_Block_InputPressed = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Player.Block.InputPressed"));
	GameplayTags.Player_Block_InputHeld = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Player.Block.InputHeld"));
	GameplayTags.Player_Block_InputReleased = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Player.Block.InputReleased"));
	GameplayTags.Player_Block_CursorTrace = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Player.Block.CursorTrace"));
	GameplayTags.Player_Block_Rotation = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Player.Block.Rotation"));

	GameplayTags.Player_EndExisting_Highlight = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Player.EndExisting.Highlight"));
}
