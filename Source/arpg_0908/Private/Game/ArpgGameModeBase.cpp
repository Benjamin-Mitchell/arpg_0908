// Copyright Tinzie Games


#include "Game/ArpgGameModeBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "EngineUtils.h"
#include "Game/ArpgGameInstance.h"
#include "Game/ArpgGameState.h"
#include "Game/ArpgPlayerStart.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Character/arpgCharacter.h"


AActor* AArpgGameModeBase::FindPlayerStart_Implementation(AController* Player, const FString& IncomingName)
{
	//When level is begun, spawn players properly.
	if (LevelBegun)
	{
		TArray<AArpgPlayerStart*> PlayerStarts;
		for (TActorIterator<AArpgPlayerStart> It(GetWorld()); It; ++It)
		{
			PlayerStarts.Add(*It);
		}

		if (PlayerStarts[PlayerSpawnStartOffset]->IsIntroStart)
			PlayerSpawnStartOffset++;

		AArpgPlayerStart* ReturnPlayerStart = PlayerStarts[PlayerSpawnStartOffset];
		PlayerSpawnStartOffset++;

		return ReturnPlayerStart;
	}
	else
	{
		TArray<APlayerStart*> PlayerStarts;
		for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
		{
			PlayerStarts.Add(*It);
		}
		//TSubclassOf<APlayerStart> PlayerStartClass = APlayerStart::StaticClass();
		//UWorld* TempWorld = GetWorld();
		//UGameplayStatics::GetAllActorsOfClass(TempWorld, APlayerStart::StaticClass(), PlayerStarts);

		for (AActor* PlayerStart : PlayerStarts)
		{
			AArpgPlayerStart* ArpgPlayerStart = Cast<AArpgPlayerStart>(PlayerStart);

			if (ArpgPlayerStart->IsIntroStart)
			{
				return ArpgPlayerStart;
			}
		}
	}

	//LevelBeginBP();

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("ERROR in GameModeBase FindPlayerStart"));
	return Super::FindPlayerStart_Implementation(Player, IncomingName);
		
}

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

void AArpgGameModeBase::LevelBegin()
{
	LevelBegun = true;
	LevelBeginBP();
}


void AArpgGameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	UArpgGameInstance *ArpgGameInstance = Cast<UArpgGameInstance>(GetGameInstance());

	bool ReadyToStart = ArpgGameInstance->AreAllPlayersConnected(GetNumPlayers());

	if (ReadyToStart)
	{
		LevelBegin();
	}

	//Special handling for if we are playing a level directly
#if WITH_EDITOR

	//If we haven't through the introMap, are not current in the intro map, and this is the first player to connect 
	if (!ArpgGameInstance->HasBeenThroughIntroMap && GetWorld()->GetMapName() != FString("UEDPIE_0_IntroMap") && GetNumPlayers() == 2)
	{
		//Then we can start a timer to trigger the game start logic manually.
		//We use a counter to allow time for all clients to join in Editor.
		GetWorld()->GetTimerManager().ClearTimer(EditorGameStartTimerHandle);
	
		// Set a timer for when countdown ends
		GetWorld()->GetTimerManager().SetTimer(
			CountdownTimerHandle,
			this,
			&AArpgGameModeBase::LevelBegin,
			1.0,
			false // Don't loop
		);	
	}


		
#endif

	
}

void AArpgGameModeBase::SpawnPlayersManually()
{
	if (UArpgGameInstance* ArpgGameInstance = Cast<UArpgGameInstance>(GetGameInstance()))
	{
		for (int i = 0; i < GetNumPlayers(); i++)
		{
			if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), i))
			{
				AActor* PlayerStartActor = FindPlayerStart(PlayerController);
				FTransform SpawnTransform = PlayerStartActor->GetActorTransform();
				APawn* SpawnedCharacter = GetWorld()->SpawnActor<APawn>(CharacterClassToSpawn, SpawnTransform);

				 UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SpawnedCharacter);
				
				PlayerController->Possess(SpawnedCharacter);
				
				if (AarpgCharacterBase* CharacterBase = Cast<AarpgCharacterBase>(SpawnedCharacter))
				{
					CharacterBase->OnCustomSpawned();
				}
				// FGameplayAbilitySpec SpawnSpec(SpawnAbilityClass);
				// ASC->GiveAbilityAndActivateOnce();
				

				//PlayerController->SpawnAnimOrSomething
			}
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


