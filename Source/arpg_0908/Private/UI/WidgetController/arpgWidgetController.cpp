// Copyright Tinzie Games


#include "UI/WidgetController/arpgWidgetController.h"

void UarpgWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WidgetControllerParams)
{
	PlayerController = WidgetControllerParams.PlayerController;
	PlayerState = WidgetControllerParams.PlayerState;
	AbilitySystemComponent = WidgetControllerParams.AbilitySystemComponent;
	AttributeSet = WidgetControllerParams.AttributeSet;
}

void UarpgWidgetController::BroadcastInitialValues()
{
	
}

void UarpgWidgetController::BindCallbacksToDependencies()
{
}
