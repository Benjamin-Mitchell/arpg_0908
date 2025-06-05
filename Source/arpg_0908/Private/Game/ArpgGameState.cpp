// Copyright Tinzie Games


#include "Game/ArpgGameState.h"

#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "Game/ArpgGameInstance.h"
#include "Game/ArpgGameModeBase.h"
#include "Net/UnrealNetwork.h"
#include "UI/HUD/arpgHUD.h"

void AArpgGameState::MulticastPassCardChoicesToWidgets_Implementation(FCardDecisions CardDecisions)
{
	APlayerController* LocalPlayerController = GetWorld()->GetFirstLocalPlayerFromController()->GetPlayerController(GetWorld());

	AarpgHUD* ArpgHUD = Cast<AarpgHUD>(LocalPlayerController->GetHUD());

	FCardProperties FirstCardProperties =	CardProgressData->CardPropertiesByStage[CardDecisions.CurrentStage].CardPropertiesPossibilities[CardDecisions.FirstCardChoiceIndex];
	FCardProperties SecondCardProperties = CardProgressData->CardPropertiesByStage[CardDecisions.CurrentStage].CardPropertiesPossibilities[CardDecisions.SecondCardChoiceIndex];
	FCardProperties ThirdCardProperties =	CardProgressData->CardPropertiesByStage[CardDecisions.CurrentStage].CardPropertiesPossibilities[CardDecisions.ThirdCardChoiceIndex];

	ArpgHUD->DisplayCards(FirstCardProperties, SecondCardProperties, ThirdCardProperties);
}


void AArpgGameState::MulticastDistributeVotes_Implementation(int FirstCardVotes, int SecondCardVotes, int ThirdCardVotes)
{
	APlayerController* LocalPlayerController = GetWorld()->GetFirstLocalPlayerFromController()->GetPlayerController(GetWorld());

	AarpgHUD* ArpgHUD = Cast<AarpgHUD>(LocalPlayerController->GetHUD());

	ArpgHUD->UpdateVotes(FirstCardVotes, SecondCardVotes, ThirdCardVotes);
}

void AArpgGameState::MulticastStartNextLevelCountdownTimer_Implementation(int CountdownDuration)
{
	RemainingSecondsOnCountdown = CountdownDuration + 1;
	UpdateNextLevelCountdown();
	GetWorld()->GetTimerManager().ClearTimer(CountdownTimerPerSecondHandle);
	// Start the countdown
	GetWorld()->GetTimerManager().SetTimer(
		CountdownTimerPerSecondHandle,
		this,
		&AArpgGameState::UpdateNextLevelCountdown,
		1.0f, // Update every second
		true // Loop
	);
}

void AArpgGameState::UpdateNextLevelCountdown()
{
	RemainingSecondsOnCountdown--;
	APlayerController* LocalPlayerController = GetWorld()->GetFirstLocalPlayerFromController()->GetPlayerController(GetWorld());
	AarpgHUD* ArpgHUD = Cast<AarpgHUD>(LocalPlayerController->GetHUD());
	ArpgHUD->UpdateNextLevelCountdown(RemainingSecondsOnCountdown);
}

void AArpgGameState::PassVoteToGameMode(int voteIndex, APlayerController* PlayerController)
{
	OnVote.Broadcast(voteIndex, PlayerController);
}

