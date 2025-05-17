// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "Game/CardProgressData.h"
#include "UI/Widget/arpgUserWidget.h"
#include "ArpgOverlayWidget.generated.h"



/**
 * 
 */
UCLASS()
class ARPG_0908_API UArpgOverlayWidget : public UarpgUserWidget
{
	GENERATED_BODY()

public:
	void AssignAndDisplayCards(FCardProperties InFirstCardProperties, FCardProperties InSecondCardProperties, FCardProperties InThirdCardProperties);

	//void UpdateVotes(int InFirstCardVotes, int InSecondCardVotes, int InThirdCardVotes);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateVotes(int InFirstCardVotes, int InSecondCardVotes, int InThirdCardVotes);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateNextLevelCountdown(int RemainingSeconds);

	UFUNCTION(BlueprintImplementableEvent)
	void SetPlayerReadyCountUIEnabled(bool InEnabled);
protected:
	UFUNCTION(BlueprintImplementableEvent)
	void DisplayCards();

	UPROPERTY(BlueprintReadOnly)
	FCardProperties FirstCardProperties;

	UPROPERTY(BlueprintReadOnly)
	FCardProperties SecondCardProperties;

	UPROPERTY(BlueprintReadOnly)
	FCardProperties ThirdCardProperties;

	UPROPERTY(BlueprintReadOnly)
	int FirstCardVotes;

	UPROPERTY(BlueprintReadOnly)
	int SecondCardVotes;

	UPROPERTY(BlueprintReadOnly)
	int ThirdCardVotes;
	
};
