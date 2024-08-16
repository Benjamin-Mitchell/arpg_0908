// Copyright Tinzie Games


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/arpgAttributeSet.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	const UarpgAttributeSet* arpgAttributeSet = CastChecked<UarpgAttributeSet>(AttributeSet);
	
	OnHealthChanged.Broadcast(arpgAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(arpgAttributeSet->GetMaxHealth());
}
