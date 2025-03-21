// Copyright Tinzie Games


#include "AbilitySystem/arpgAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "ArpgAbilityTypes.h"
#include "ArpgGameplayTags.h"
#include "GameFramework/Character.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystem/ArpgAbilitySystemLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Net/NetworkProfiler.h"
#include "Net/UnrealNetwork.h"
#include "Player/arpgPlayerController.h"

UarpgAttributeSet::UarpgAttributeSet()
{
	
}

void UarpgAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UarpgAttributeSet, AttackDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UarpgAttributeSet, AbilityPower, COND_None, REPNOTIFY_Always);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UarpgAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UarpgAttributeSet, DamageReduction, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UarpgAttributeSet, MoveSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UarpgAttributeSet, CritChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UarpgAttributeSet, CritDamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UarpgAttributeSet, NumberOfDashes, COND_None, REPNOTIFY_Always);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UarpgAttributeSet, Health, COND_None, REPNOTIFY_Always);

	//Old From tutorial
	DOREPLIFETIME_CONDITION_NOTIFY(UarpgAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UarpgAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UarpgAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UarpgAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UarpgAttributeSet, Vigor, COND_None, REPNOTIFY_Always);
}

//TODO: Clamping in this doesn't work properly -- it only clamps the result of this modifier, not the actual Attribute value. Still useful-ish.
void UarpgAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if(Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	
	if(Attribute == GetManaAttribute())
    {
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxMana());
    }
}



void UarpgAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const
{
	//Source = causer of the effect, Target = target of the effect (owner of this AS)
		
	Props.EffectContextHandle = Data.EffectSpec.GetContext();
	Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();

	if(IsValid(Props.SourceASC) && Props.SourceASC->AbilityActorInfo.IsValid() && Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();
		Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();
		if(Props.SourceController == nullptr && Props.SourceAvatarActor != nullptr)
		{
			if(const APawn* Pawn = Cast<APawn>(Props.SourceAvatarActor))
			{
				Props.SourceController = Pawn->GetController();
			}
		}
		if(Props.SourceController)
		{
			Props.SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
		}
	}

	if(Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);
		Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent((Props.TargetAvatarActor));
	}
}

void UarpgAttributeSet::Debuff(const FEffectProperties& EffectProps)
{
	const FArpgGameplayTags& GameplayTags = FArpgGameplayTags::Get();
	FGameplayEffectContextHandle EffectContext = EffectProps.SourceASC->MakeEffectContext();
	EffectContext.AddSourceObject(EffectProps.SourceAvatarActor);

	const FGameplayTag DebuffTag = UArpgAbilitySystemLibrary::GetDebuffTag(EffectProps.EffectContextHandle);
	const float DebuffDamage = UArpgAbilitySystemLibrary::GetDebuffDamage(EffectProps.EffectContextHandle);
	const float DebuffDuration = UArpgAbilitySystemLibrary::GetDebuffDuration(EffectProps.EffectContextHandle);
	const float DebuffFrequency = UArpgAbilitySystemLibrary::GetDebuffFrequency(EffectProps.EffectContextHandle);

	FString DebuffName = FString::Printf(TEXT("DynamicDebuff_%s"), *DebuffTag.ToString());
	UGameplayEffect* Effect = NewObject<UGameplayEffect>(GetTransientPackage(), FName(DebuffName));

	Effect->DurationPolicy = EGameplayEffectDurationType::HasDuration;
	Effect->Period = DebuffFrequency;
	Effect->DurationMagnitude = FScalableFloat(DebuffDuration);

	FInheritedTagContainer TagContainer = FInheritedTagContainer();
	UTargetTagsGameplayEffectComponent& Component = Effect->FindOrAddComponent<UTargetTagsGameplayEffectComponent>();
	TagContainer.AddTag(DebuffTag);
	Component.SetAndApplyTargetTagChanges(TagContainer);

	Effect->StackingType = EGameplayEffectStackingType::AggregateBySource;
	Effect->StackLimitCount = 1;

	const int32 Index = Effect->Modifiers.Num();
	Effect->Modifiers.Add(FGameplayModifierInfo());
	FGameplayModifierInfo& ModifierInfo = Effect->Modifiers[Index];

	ModifierInfo.ModifierMagnitude = FScalableFloat(DebuffDamage);
	ModifierInfo.ModifierOp = EGameplayModOp::Additive;
	ModifierInfo.Attribute = UarpgAttributeSet::GetIncomingDamageAttribute();

	if (FGameplayEffectSpec* MutableSpec = new FGameplayEffectSpec(Effect, EffectContext, 1.f))
	{
		FArpgGameplayEffectContext* AuraContext = static_cast<FArpgGameplayEffectContext*>(MutableSpec->GetContext().Get());
		TSharedPtr<FGameplayTag> DebuffTagShared = MakeShareable(new FGameplayTag(DebuffTag));
		AuraContext->SetDebuffTag(DebuffTagShared);

		EffectProps.TargetASC->ApplyGameplayEffectSpecToSelf(*MutableSpec);
	}
}

void UarpgAttributeSet::HandleIncomingDamage(const FEffectProperties& EffectProps)
{
	const float LocalIncomingDamage = GetIncomingDamage();
	SetIncomingDamage(0.f); //we always reset meta attributes before next use
	if(LocalIncomingDamage > 0.f)
	{
		const float NewHealth = GetHealth() - LocalIncomingDamage;
		SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));

		const bool bFatal = NewHealth <= 0.f;

		if(bFatal)
		{
			ICombatInterface* CombatInterface = Cast<ICombatInterface>(EffectProps.TargetAvatarActor);
			if(CombatInterface)
			{
				FVector Impulse = UArpgAbilitySystemLibrary::GetDeathImpulse(EffectProps.EffectContextHandle);
				CombatInterface->Die(Impulse);
			}
		}
		else
		{
			const FVector& KnockbackForce =UArpgAbilitySystemLibrary::GetKnockbackForce(EffectProps.EffectContextHandle); 
			if (!KnockbackForce.IsNearlyZero(1.f))
			{
				EffectProps.TargetCharacter->GetCharacterMovement()->StopMovementImmediately();
				EffectProps.TargetCharacter->LaunchCharacter(KnockbackForce, false, false);
			}
			
			//Activate any abilities with the "effects_hitReact" tag on the target
			FGameplayTagContainer TagContainer;
			TagContainer.AddTag(FArpgGameplayTags::Get().Effects_HitReact);
			EffectProps.TargetASC->TryActivateAbilitiesByTag(TagContainer);
		}

			
		const bool bBlocked = UArpgAbilitySystemLibrary::IsBlockedHit(EffectProps.EffectContextHandle);
		const bool bCriticalHit = UArpgAbilitySystemLibrary::IsCriticalHit(EffectProps.EffectContextHandle);
		ShowFloatingText(EffectProps, LocalIncomingDamage, bBlocked, bCriticalHit);
	}
}


void UarpgAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FEffectProperties EffectProps;
	SetEffectProperties(Data, EffectProps);

	if (EffectProps.TargetCharacter->Implements<UCombatInterface>() && ICombatInterface::Execute_IsDead(EffectProps.TargetCharacter))
	{
		return;
	}

	//Extra clamping (for base value?)
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
		UE_LOG(LogTemp, Warning, TEXT("Changed Health on %s, Health: %f"), *EffectProps.TargetAvatarActor->GetName(), GetHealth());
	}
	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}

	if(Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		HandleIncomingDamage(EffectProps);

		if (UArpgAbilitySystemLibrary::IsSuccessfulDebuff(EffectProps.EffectContextHandle))
		{
			Debuff(EffectProps);
		}
		
	}
	
}

void UarpgAttributeSet::ShowFloatingText(const FEffectProperties& EffectProps, float Damage, bool bBlockedHit, bool bCriticalHit) const
{
	if(EffectProps.SourceCharacter != EffectProps.TargetCharacter)
	{
		if(AarpgPlayerController* PlayerController = Cast<AarpgPlayerController>(EffectProps.SourceCharacter->Controller))
		{
			PlayerController->ShowDamageNumber(Damage, EffectProps.TargetCharacter, bBlockedHit, bCriticalHit);
			return;
		}
		
		if(AarpgPlayerController* PlayerController = Cast<AarpgPlayerController>(EffectProps.TargetCharacter->Controller))
		{
			PlayerController->ShowDamageNumber(Damage, EffectProps.TargetCharacter, bBlockedHit, bCriticalHit);
		}
				
	}
}

void UarpgAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	//Notify the gameplay system that this attribute has been updated.
	GAMEPLAYATTRIBUTE_REPNOTIFY(UarpgAttributeSet, Health, OldHealth);
}

void UarpgAttributeSet::OnRep_AttackDamage(const FGameplayAttributeData& OldAttackDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UarpgAttributeSet, AttackDamage, OldAttackDamage);
}

void UarpgAttributeSet::OnRep_AbilityPower(const FGameplayAttributeData& OldAbilityPower) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UarpgAttributeSet, AbilityPower, OldAbilityPower);
}

void UarpgAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	//Notify the gameplay system that this attribute has been updated.
	GAMEPLAYATTRIBUTE_REPNOTIFY(UarpgAttributeSet, MaxHealth, OldMaxHealth);
}

void UarpgAttributeSet::OnRep_DamageReduction(const FGameplayAttributeData& OldDamageReduction) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UarpgAttributeSet, DamageReduction, OldDamageReduction);
}

void UarpgAttributeSet::OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UarpgAttributeSet, BlockChance, OldBlockChance);
}

void UarpgAttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UarpgAttributeSet, MoveSpeed, OldMoveSpeed);
}

void UarpgAttributeSet::OnRep_CritChance(const FGameplayAttributeData& OldCritChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UarpgAttributeSet, CritChance, OldCritChance);
}

void UarpgAttributeSet::OnRep_CritDamageMultiplier(const FGameplayAttributeData& OldCritDamageMultiplier) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UarpgAttributeSet, CritDamageMultiplier, OldCritDamageMultiplier);
}

void UarpgAttributeSet::OnRep_NumberOfDashes(const FGameplayAttributeData& OldNumberOfDashes) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UarpgAttributeSet, NumberOfDashes, OldNumberOfDashes);
}

void UarpgAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	//Notify the gameplay system that this attribute has been updated.
	GAMEPLAYATTRIBUTE_REPNOTIFY(UarpgAttributeSet, Mana, OldMana);
}

void UarpgAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	//Notify the gameplay system that this attribute has been updated.
	GAMEPLAYATTRIBUTE_REPNOTIFY(UarpgAttributeSet, MaxMana, OldMaxMana);
}

void UarpgAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UarpgAttributeSet, Strength, OldStrength);
}

void UarpgAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UarpgAttributeSet, Intelligence, OldIntelligence);
}

void UarpgAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldVigor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UarpgAttributeSet, Vigor, OldVigor);
}



