// Copyright Tinzie Games


#include "AbilitySystem/arpgAttributeSet.h"
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

void UarpgAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if(Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
		
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Health NewValue: %f"), NewValue));
	}
	
	if(Attribute == GetManaAttribute())
    {
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxMana());
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
