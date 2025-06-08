// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "CardProgressData.h"

#include "GameFramework/GameModeBase.h"
#include "ArpgGameModeBase.generated.h"


class UCameraComponent;
class ULevelSequence;
class AarpgPlayerController;
class AarpgCharacter;
class UHeadData;
class UCharacterClassInfo;
struct FCardDecisions;



/**
 * 
 */
UCLASS()
class ARPG_0908_API AArpgGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults")
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;

	UPROPERTY(EditAnywhere, Category="Character Class Defaults")
	TSubclassOf<AarpgCharacter> CharacterClassToSpawn;
	//Currently storing this on the character to keep it simple for repeated access.
	//If more things need access, move it here in the future.
	// UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults")
	// TObjectPtr<UHeadData> HeadDatabase;

	UFUNCTION(BlueprintImplementableEvent)
	void LevelBeginBP();

	virtual AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName = TEXT("")) override;
	
protected:

	UFUNCTION(BlueprintCallable)
	void LevelComplete();

	void ReceiveVote(int index, APlayerController* CallingController);
	
	// Countdown variables
	UPROPERTY(EditAnywhere, Category = "Countdown")
	float NextLevelCountdownDuration = 3.0f; // Default 3 seconds
	
	virtual void OnPostLogin(AController* NewPlayer) override;


	UFUNCTION(BlueprintCallable)
	TArray<AarpgPlayerController*> SpawnPlayersManually();
	
	UFUNCTION(BlueprintCallable)
	void BeginIntroCinematic(ULevelSequence* LevelSequenceAsset, FTransform SpawnTransform);

	UFUNCTION(BlueprintImplementableEvent)
	void IntroEnded();

	UPROPERTY(EditDefaultsOnly)
	bool DebugSkipCinematic = false;

	UPROPERTY(EditDefaultsOnly)
	int NumDebugPlayers = 2;
private:
	bool LevelCompleted = false;

	TMap<uint32, int> Votes;
	FTimerHandle CountdownTimerHandle;

	int RemainingSeconds;
	void StartNextLevelCountdown(int HighestVoteIndex);
	void OnNextLevelCountdownEnded(int HighestVoteIndex);

	void LevelBegin();
	bool LevelBegun = false;

	int PlayerSpawnStartOffset = 0;

	UFUNCTION()
	void HandleIntroCameraCut(UCameraComponent* CameraComponent);

	UFUNCTION()
	void HandleIntroEnd();
	
	
#if WITH_EDITOR
	FTimerHandle EditorGameStartTimerHandle;
#endif
};

