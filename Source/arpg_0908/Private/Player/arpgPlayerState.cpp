// Copyright Tinzie Games

#include "Player/arpgPlayerState.h"

#include "AbilitySystem/arpgAbilitySystemComponent.h"
#include "AbilitySystem/arpgAttributeSet.h"

AarpgPlayerState::AarpgPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UarpgAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);

	AttributeSet = CreateDefaultSubobject<UarpgAttributeSet>("AttributeSet");

	NetUpdateFrequency = 100.0f;
}

UAbilitySystemComponent* AarpgPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
