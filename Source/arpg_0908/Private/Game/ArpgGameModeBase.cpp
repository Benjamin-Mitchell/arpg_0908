// Copyright Tinzie Games


#include "Game/ArpgGameModeBase.h"

#include <string>

#include "AudioMixerBlueprintLibrary.h"
#include "Game/ArpgGameInstance.h"
#include "Game/ArpgGameState.h"
#include "UI/HUD/arpgHUD.h"

void AArpgGameModeBase::LevelComplete()
{
	if (LevelCompleted)
		return;

	LevelCompleted = true;
		
	UArpgGameInstance *ArpgGameInstance = Cast<UArpgGameInstance>(GetGameInstance());

	FCardDecisions CurrentCardDecisions; 
	ArpgGameInstance->NotifyLevelComplete(CurrentCardDecisions);


	AArpgGameState* ArpgGameState = GetGameState<AArpgGameState>();
	ArpgGameState->MulticastPassCardChoicesToWidgets(CurrentCardDecisions);
	ArpgGameState->OnVote.AddUObject(this, &AArpgGameModeBase::ReceiveVote);
}

void AArpgGameModeBase::ReceiveVote(int index, APlayerController* CallingController)
{
	uint32 PlayerID = CallingController->GetUniqueID();

	//First we record the vote
	if (Votes.Contains(PlayerID))
	{
		Votes[PlayerID] = index;
	}
	else
		Votes.Add(PlayerID, index);

	//Then we update the visual for all clients	
	AArpgGameState* ArpgGameState = GetGameState<AArpgGameState>();

	TArray<int> AccumulatedVotes;
	AccumulatedVotes.SetNum(3);
	for (const auto& Pair : Votes)
	{
		int32 Value = Pair.Value;
		AccumulatedVotes[Value]++;
	}
	
	ArpgGameState->MulticastDistributeVotes(AccumulatedVotes[0], AccumulatedVotes[1], AccumulatedVotes[2]);

	
	//Then we determine if a new level is chosen
	if (Votes.Num() == GetWorld()->GetNumPlayerControllers())
	{
		int MaxVotes = 0;
		int HighestVoteIndex = -1;
		bool Stalemate = false;
		for (int i = 0; i < AccumulatedVotes.Num(); i++)
		{
			int VoteCount = AccumulatedVotes[i];
			
			if (VoteCount > MaxVotes)
			{
				MaxVotes = VoteCount;
				HighestVoteIndex = i;
				Stalemate = false;
			}
			else if (VoteCount == MaxVotes)
			{
				Stalemate = true;
			}
			
		}

		if (!Stalemate)
		{
			StartNextLevelCountdown(HighestVoteIndex);
		}
	}
	
}

void AArpgGameModeBase::StartNextLevelCountdown(int HighestVoteIndex)
{
	// Clear existing timer just in case
	GetWorld()->GetTimerManager().ClearTimer(CountdownTimerHandle);

	//Tell GameState that Countdown is updated. GameState updates Widgets
	AArpgGameState* ArpgGameState = GetGameState<AArpgGameState>();
	ArpgGameState->MulticastStartNextLevelCountdownTimer(NextLevelCountdownDuration);

	// Set a timer for when countdown ends
	GetWorld()->GetTimerManager().SetTimer(
		CountdownTimerHandle,
		[this, HighestVoteIndex]() 
			{
				OnNextLevelCountdownEnded(HighestVoteIndex);
			},
		NextLevelCountdownDuration,
		false // Don't loop
	);
}


void AArpgGameModeBase::OnNextLevelCountdownEnded(int HighestVoteIndex)
{	
	UArpgGameInstance *ArpgGameInstance = Cast<UArpgGameInstance>(GetGameInstance());

	ArpgGameInstance->NotifyLevelSelectionConfirmed(HighestVoteIndex);
	
}


