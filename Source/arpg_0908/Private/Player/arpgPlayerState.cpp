// Copyright Tinzie Games

#include "Player/arpgPlayerState.h"

#include "OnlineSubsystem.h"
#include "AbilitySystem/arpgAbilitySystemComponent.h"
#include "AbilitySystem/arpgAttributeSet.h"
#include "Game/ArpgGameInstance.h"
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

//Called on server and value is replicated.
void AarpgPlayerState::SetEquippedWeapon(int WeaponIndex)
{
	UArpgGameInstance* ArpgGameInstance = Cast<UArpgGameInstance>(GetGameInstance());
	ArpgGameInstance->SetPlayerEquippedWeapon(GetUniqueId().GetUniqueNetId(), WeaponIndex);
}
int AarpgPlayerState::GetEquippedWeaponIndex() const
{
	UArpgGameInstance* ArpgGameInstance = Cast<UArpgGameInstance>(GetGameInstance());
	return ArpgGameInstance->GetPlayerEquippedWeapon(GetUniqueId().GetUniqueNetId());
}

//Called on server and value is replicated.
void AarpgPlayerState::SetEquippedHead(int HeadIndex)
{
	UArpgGameInstance* ArpgGameInstance = Cast<UArpgGameInstance>(GetGameInstance());
	ArpgGameInstance->SetPlayerEquippedHead(GetUniqueId().GetUniqueNetId(), HeadIndex);
}

int AarpgPlayerState::GetEquippedHeadIndex() const
{
	UArpgGameInstance* ArpgGameInstance = Cast<UArpgGameInstance>(GetGameInstance());
	return ArpgGameInstance->GetPlayerEquippedHead(GetUniqueId().GetUniqueNetId());
}

void AarpgPlayerState::OnRep_Level(int32 OldLevel)
{
	
}
