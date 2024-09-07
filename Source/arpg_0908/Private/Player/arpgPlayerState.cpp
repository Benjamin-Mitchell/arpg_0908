// Copyright Tinzie Games

#include "Player/arpgPlayerState.h"

#include "AbilitySystem/arpgAbilitySystemComponent.h"
#include "AbilitySystem/arpgAttributeSet.h"
#include "Net/UnrealNetwork.h"

AarpgPlayerState::AarpgPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UarpgAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UarpgAttributeSet>("AttributeSet");

	NetUpdateFrequency = 100.0f;
}

void AarpgPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AarpgPlayerState, Level);
}

UAbilitySystemComponent* AarpgPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AarpgPlayerState::OnRep_Level(int32 OldLevel)
{
	
}
