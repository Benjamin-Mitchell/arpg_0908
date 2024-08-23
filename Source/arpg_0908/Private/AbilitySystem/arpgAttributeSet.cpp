// Copyright Tinzie Games


#include "AbilitySystem/arpgAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "GameplayEffectExtension.h"
#include "Net/NetworkProfiler.h"
#include "Net/UnrealNetwork.h"

UarpgAttributeSet::UarpgAttributeSet()
{
	InitHealth(50.0f);
	InitMaxHealth(100.0f);
	InitMana(25.0f);
	InitMaxMana(100.0f);
}

void UarpgAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Registry health to be replicated
	DOREPLIFETIME_CONDITION_NOTIFY(UarpgAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UarpgAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UarpgAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UarpgAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
}

//TODO: Clamping in this doesn't work properly -- doesn't affect Base value?
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


void UarpgAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FEffectProperties EffectProps;
	SetEffectProperties(Data, EffectProps);


	//Extra clamping (for base value?)
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}
	
}

void UarpgAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	//Notify the gameplay system that this attribute has been updated.
	GAMEPLAYATTRIBUTE_REPNOTIFY(UarpgAttributeSet, Health, OldHealth);
}

void UarpgAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	//Notify the gameplay system that this attribute has been updated.
	GAMEPLAYATTRIBUTE_REPNOTIFY(UarpgAttributeSet, MaxHealth, OldMaxHealth);
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

