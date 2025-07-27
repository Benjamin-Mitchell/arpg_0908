// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "CardProgressData.h"
#include "Engine/GameInstance.h"
#include "ArpgGameInstance.generated.h"

enum EGameState
{
	GameState_Menu,
	GameState_GameClient,
	GameState_GameHost
};

USTRUCT()
struct FCardDecisions
{
	GENERATED_BODY()

	// Add this to enable network serialization
	//bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	UPROPERTY()	//MUST BE UPROPERTIES FOR SERIALIZATION
	int CurrentStage = 0;
	
	UPROPERTY()
	int FirstCardChoiceIndex = -1;

	UPROPERTY()
	int SecondCardChoiceIndex = -1;
	
	UPROPERTY()
	int ThirdCardChoiceIndex = -1;
};

USTRUCT()
struct FPersistentPlayerData
{
	GENERATED_BODY()
	
	int EquippedWeaponIndex = -1;

	int EquippedHeadIndex = -1;
};

// Custom hasher for FUniqueNetIdPtr
struct FUniqueNetIdPtrHasher
{
	uint32 operator()(const FUniqueNetIdPtr& NetId) const
	{
		if (NetId.IsValid())
		{
			return GetTypeHash(NetId->ToString()); // Hash the string representation
		}
		return 0;
	}
};

// Custom equality comparator
struct FUniqueNetIdPtrEqual
{
	bool operator()(const FUniqueNetIdPtr& A, const FUniqueNetIdPtr& B) const
	{
		if (A.IsValid() && B.IsValid())
		{
			return *A == *B; // Compare the actual IDs
		}
		return false;
	}
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

	void NotifyLevelComplete(FCardDecisions& OutCurrentCardDecisions);

	UFUNCTION(BlueprintCallable, Category = "GameStart")
	void NotifyPlayerReadyComplete(int NumPlayers);

	UFUNCTION(BlueprintCallable, Category = "GameStart")
	bool AreAllPlayersConnected(int InNumPlayers);
	
	void NotifyLevelSelectionConfirmed(int CardIndex);

	bool HasBeenThroughIntroMap = false;

	bool HasBeenThroughMainMenu = false;

	UFUNCTION(BlueprintCallable, Category = "GameStart")
	void SetBeenThroughMainMenu();

	void SetPlayerEquippedWeapon(FUniqueNetIdPtr InID, int EquippedWeaponID);
	int GetPlayerEquippedWeapon(FUniqueNetIdPtr InID);
	
	void SetPlayerEquippedHead(FUniqueNetIdPtr InID, int EquippedHeadID);
	int GetPlayerEquippedHead(FUniqueNetIdPtr InID);
protected:
	virtual void Init() override;
	virtual void Shutdown() override;

	UPROPERTY(EditDefaultsOnly, Category = "Game Data")
	TObjectPtr<UCardProgressData> CardProgressData;
	
private:

	int NumPlayersThisGame;
	int CurrentStage;
	EGameState GameState;

	FCardDecisions CurrentCardDecisions;
	FCardDecisions MakeCardDecisions();

	void LoadMapWithCallback(const FString& MapName);

	//TMap<FUniqueNetIdPtr, FPersistentPlayerData, FUniqueNetIdPtrHasher, FUniqueNetIdPtrEqual> PersistentPlayerDatas;
	TMap<FString, FPersistentPlayerData> PersistentPlayerDatas;
	
};
