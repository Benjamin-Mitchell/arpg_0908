// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "arpgHUD.generated.h"

class UArpgOverlayWidget;
struct FCardProperties;
class UAttributeSet;
class UAbilitySystemComponent;
struct FWidgetControllerParams;
class UOverlayWidgetController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWidgetInitialized);


/**
 * 
 */
UCLASS()
class ARPG_0908_API AarpgHUD : public AHUD
{
	GENERATED_BODY()
public:

	UPROPERTY()
	TObjectPtr<UArpgOverlayWidget> OverlayWidget;

	UOverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams& WidgetControllerParams);

	void InitOverlay(APlayerController* PlayerController, APlayerState* PlayerState, UAbilitySystemComponent* AbilitySystemComponent, UAttributeSet* AttributeSet);

	void DisplayCards(FCardProperties FirstCardProperties, FCardProperties SecondCardProperties, FCardProperties ThirdCardProperties);

	void UpdateVotes(int FirstCardVotes, int SecondCardVotes, int ThirdCardVotes);

	void UpdateNextLevelCountdown(int RemainingSeconds);

	UPROPERTY(BlueprintAssignable, Category="Initialization")
	FOnWidgetInitialized OnWidgetInitialized;

	UFUNCTION(BlueprintCallable)
	void SetPlayerReadyCountUIEnabled(bool InEnabled);
private:

	UPROPERTY(EditAnywhere)
	TSubclassOf<UArpgOverlayWidget> OverlayWidgetClass;

	UPROPERTY()
	TObjectPtr<UOverlayWidgetController> OverlayWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;
};
