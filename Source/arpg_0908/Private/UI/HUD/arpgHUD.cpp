// Copyright Tinzie Games


#include "UI/HUD/arpgHUD.h"

#include "UI/Widget/arpgUserWidget.h"

void AarpgHUD::BeginPlay()
{
	Super::BeginPlay();

	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), OverlayWidgetClass);
	Widget->AddToViewport();
}
