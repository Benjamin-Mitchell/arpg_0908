// Copyright Tinzie Games


#include "Game/ArpgGameInstance.h"

UArpgGameInstance::UArpgGameInstance()
{
}

void UArpgGameInstance::JoinedGame()
{
	GameState = EGameState::GameState_GameClient;
}

void UArpgGameInstance::HostedGame()
{
	GameState = EGameState::GameState_GameHost;
}

void UArpgGameInstance::LeftGame()
{
	GameState = EGameState::GameState_Menu;
}

void UArpgGameInstance::Init()
{
	Super::Init();
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Instance Init"));
}

void UArpgGameInstance::Shutdown()
{
	Super::Shutdown();
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Instance Shutdown"));
}
