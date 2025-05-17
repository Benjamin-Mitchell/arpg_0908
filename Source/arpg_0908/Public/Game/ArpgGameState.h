// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "CardProgressData.h"
#include "GameFramework/GameState.h"
#include "ArpgGameState.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnVoteCast, int, APlayerController*);

struct FCardDecisions;

USTRUCT(BlueprintType)
struct FVoteStruct
{
	GENERATED_BODY()
	
	APlayerController* PlayerController;

	int PlayerVotedIndex;
};

/**
 * 
 */
UCLASS()
class ARPG_0908_API AArpgGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastPassCardChoicesToWidgets(FCardDecisions CardDecisions);

	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastDistributeVotes(int FirstCardVotes, int SecondCardVotes, int ThirdCardVotes);

	void UpdateNextLevelCountdown();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStartNextLevelCountdownTimer(int CountdownDuration);
	
	void PassVoteToGameMode(int voteIndex, APlayerController* PlayerController);
	
	FOnVoteCast OnVote;
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Game Data")
	TObjectPtr<UCardProgressData> CardProgressData;


	int RemainingSecondsOnCountdown;
	FTimerHandle CountdownTimerPerSecondHandle;
	
	
};
