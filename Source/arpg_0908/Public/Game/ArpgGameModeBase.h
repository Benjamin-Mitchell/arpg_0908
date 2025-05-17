// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "CardProgressData.h"
#include "GameFramework/GameModeBase.h"
#include "ArpgGameModeBase.generated.h"



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

	//Currently storing this on the character to keep it simple for repeated access.
	//If more things need access, move it here in the future.
	// UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults")
	// TObjectPtr<UHeadData> HeadDatabase;

protected:

	UFUNCTION(BlueprintCallable)
	void LevelComplete();

	void ReceiveVote(int index, APlayerController* CallingController);
	
	// Countdown variables
	UPROPERTY(EditAnywhere, Category = "Countdown")
	float NextLevelCountdownDuration = 3.0f; // Default 3 seconds
	
private:
	bool LevelCompleted = false;

	TMap<uint32, int> Votes;
	FTimerHandle CountdownTimerHandle;

	int RemainingSeconds;
	void StartNextLevelCountdown(int HighestVoteIndex);
	void OnNextLevelCountdownEnded(int HighestVoteIndex);
};
