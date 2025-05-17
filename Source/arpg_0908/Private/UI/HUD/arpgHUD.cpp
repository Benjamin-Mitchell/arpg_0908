// Copyright Tinzie Games


#include "UI/HUD/arpgHUD.h"

#include "UI/Widget/ArpgOverlayWidget.h"
#include "UI/WidgetController/OverlayWidgetController.h"

UOverlayWidgetController* AarpgHUD::GetOverlayWidgetController(const FWidgetControllerParams& WidgetControllerParams)
{
	if(OverlayWidgetController == nullptr)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(WidgetControllerParams);

		OverlayWidgetController->BindCallbacksToDependencies();

		return OverlayWidgetController;
	}

	return OverlayWidgetController;
}

void AarpgHUD::InitOverlay(APlayerController* PlayerController, APlayerState* PlayerState,
	UAbilitySystemComponent* AbilitySystemComponent, UAttributeSet* AttributeSet)
{
	checkf(OverlayWidgetClass, TEXT("Overlay Widget Class uninitialzied, please fill out BP_arpgHUD"));
	checkf(OverlayWidgetControllerClass, TEXT("Overlay Widget Controller Class uninitialzied, please fill out BP_arpgHUD"));
	
	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), OverlayWidgetClass);
	OverlayWidget = Cast<UArpgOverlayWidget>(Widget);

	const FWidgetControllerParams WidgetControllerParams(PlayerController, PlayerState, AbilitySystemComponent, AttributeSet);
	UOverlayWidgetController* WidgetController = GetOverlayWidgetController(WidgetControllerParams);

	OverlayWidget->SetWidgetController(WidgetController);

	WidgetController->BroadcastInitialValues();

	Widget->AddToViewport();
}

void AarpgHUD::DisplayCards(FCardProperties FirstCardProperties, FCardProperties SecondCardProperties,
	FCardProperties ThirdCardProperties)
{
	OverlayWidget->AssignAndDisplayCards(FirstCardProperties, SecondCardProperties, ThirdCardProperties);
	
}

void AarpgHUD::UpdateVotes(int FirstCardVotes, int SecondCardVotes, int ThirdCardVotes)
{
	OverlayWidget->UpdateVotes(FirstCardVotes, SecondCardVotes, ThirdCardVotes);
}

void AarpgHUD::UpdateNextLevelCountdown(int RemainingSeconds)
{
	OverlayWidget->UpdateNextLevelCountdown(RemainingSeconds);
}

void AarpgHUD::SetPlayerReadyCountUIEnabled(bool InEnabled)
{
	OverlayWidget->SetPlayerReadyCountUIEnabled(InEnabled);
}
