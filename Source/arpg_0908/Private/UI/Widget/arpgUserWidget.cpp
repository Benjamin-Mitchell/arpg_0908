// Copyright Tinzie Games


#include "UI/Widget/arpgUserWidget.h"

void UarpgUserWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet();
}
