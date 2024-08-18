// Copyright Tinzie Games


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/arpgAttributeSet.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	const UarpgAttributeSet* arpgAttributeSet = CastChecked<UarpgAttributeSet>(AttributeSet);
	
	OnHealthChanged.Broadcast(arpgAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(arpgAttributeSet->GetMaxHealth());
	
	OnManaChanged.Broadcast(arpgAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(arpgAttributeSet->GetMaxMana());

}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	const UarpgAttributeSet* arpgAttributeSet = CastChecked<UarpgAttributeSet>(AttributeSet);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		arpgAttributeSet->GetHealthAttribute()).AddUObject(this, &UOverlayWidgetController::HealthChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		arpgAttributeSet->GetMaxHealthAttribute()).AddUObject(this, &UOverlayWidgetController::MaxHealthChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		arpgAttributeSet->GetManaAttribute()).AddUObject(this, &UOverlayWidgetController::ManaChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		arpgAttributeSet->GetMaxManaAttribute()).AddUObject(this, &UOverlayWidgetController::MaxManaChanged);
}

void UOverlayWidgetController::HealthChanged(const FOnAttributeChangeData& Data) const 
{
	OnHealthChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::MaxHealthChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxHealthChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::ManaChanged(const FOnAttributeChangeData& Data) const 
{
	OnManaChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::MaxManaChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxManaChanged.Broadcast(Data.NewValue);
}
