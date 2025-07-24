// Copyright Tinzie Games


#include "Game/ArpgGameInstance.h"

UArpgGameInstance::UArpgGameInstance()
{
	CurrentStage = 0;
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

void UArpgGameInstance::NotifyLevelComplete(FCardDecisions& OutCurrentCardDecisions)
{
	if (CurrentStage == CardProgressData->CardPropertiesByStage.Num())
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, TEXT("Congrats you won the game"));

	OutCurrentCardDecisions = MakeCardDecisions();
	
}

void UArpgGameInstance::NotifyPlayerReadyComplete(int NumPlayers)
{
	NumPlayersThisGame = NumPlayers;
	HasBeenThroughIntroMap = true;
}

bool UArpgGameInstance::AreAllPlayersConnected(int InNumPlayers)
{
	return InNumPlayers == NumPlayersThisGame;
}

void UArpgGameInstance::NotifyLevelSelectionConfirmed(int CardIndex)
{
	TArray<FCardProperties> CardProperties = CardProgressData->CardPropertiesByStage[CurrentStage].CardPropertiesPossibilities;
	
	TArray<FCardProperties> CardPropertiesOnSelection = {CardProperties[CurrentCardDecisions.FirstCardChoiceIndex], CardProperties[CurrentCardDecisions.FirstCardChoiceIndex], CardProperties[CurrentCardDecisions.FirstCardChoiceIndex]};
	
	FString ChosenMapName = CardPropertiesOnSelection[CardIndex].LevelName;
	
	LoadMapWithCallback(ChosenMapName);
	
	CurrentStage++;
}

void UArpgGameInstance::SetBeenThroughMainMenu()
{
	HasBeenThroughMainMenu = true;
}

void UArpgGameInstance::Init()
{
	Super::Init();
}

void UArpgGameInstance::Shutdown()
{
	Super::Shutdown();
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Instance Shutdown"));
}

FCardDecisions UArpgGameInstance::MakeCardDecisions()
{
	CurrentCardDecisions.CurrentStage = CurrentStage;
	//TODO: Logic
	CurrentCardDecisions.FirstCardChoiceIndex = 0;
	CurrentCardDecisions.SecondCardChoiceIndex = 1;
	CurrentCardDecisions.ThirdCardChoiceIndex = 2;

	return CurrentCardDecisions;
	
}


void UArpgGameInstance::LoadMapWithCallback(const FString& MapName)
{
	// Setup loading screen first if needed
    
	FString LevelPath = FString::Printf(TEXT("/Game/Maps/%s"), *MapName);
    
	// Setup the async load
	LoadPackageAsync(LevelPath,
		FLoadPackageAsyncDelegate::CreateLambda([this](const FName& PackageName, UPackage* LoadedPackage, EAsyncLoadingResult::Type Result)
		{
			if (Result == EAsyncLoadingResult::Succeeded)
			{
				GetWorld()->ServerTravel(FString::Printf(TEXT("%s?listen"), *PackageName.ToString()));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to load map: %s"), *PackageName.ToString());
			}
		}),
		0, PKG_ContainsMap);
}
