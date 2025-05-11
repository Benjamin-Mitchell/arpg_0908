// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ArpgGameInstance.generated.h"

enum EGameState
{
	GameState_Menu,
	GameState_GameHost,
	GameState_GameClient
};

/**
 * The Game Instance class is effectively a singleton and is used to manage persistent data amd game map loads. 
 * First, the Game Instance waits until the main menu is passed and a lobby is loaded.
 * The first map, IntroMap, is where players may select weapons, perhaps pick up a basic Head. It is also the entry to the game loop.
 * Following that, the loop is fairly simple. We start a game, register a delegate to the game mode.
 * When the game mode notifies instance of game completion, we restart the loop. 
 */
UCLASS()
class ARPG_0908_API UArpgGameInstance : public UGameInstance
{
	GENERATED_BODY()


public:
	UArpgGameInstance();

	UFUNCTION(BlueprintCallable, Category = "GameStart")
	void JoinedGame();
	
	UFUNCTION(BlueprintCallable, Category = "GameStart")
	void HostedGame();

	UFUNCTION(BlueprintCallable, Category = "GameStart")
	void LeftGame();
	
protected:
	virtual void Init() override;
	virtual void Shutdown() override;

	
	
private:
	EGameState GameState;
};
