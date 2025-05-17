// Copyright Tinzie Games


#include "UI/Widget/ArpgOverlayWidget.h"

void UArpgOverlayWidget::AssignAndDisplayCards(FCardProperties InFirstCardProperties,
	FCardProperties InSecondCardProperties, FCardProperties InThirdCardProperties)
{
	FirstCardProperties = InFirstCardProperties;
	SecondCardProperties = InSecondCardProperties;
	ThirdCardProperties = InThirdCardProperties;

	DisplayCards();
}
