// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "arpgUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class ARPG_0908_API UarpgUserWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SetWidgetController(UObject* InWidgetController);
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject> WidgetController;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void WidgetControllerSet();
	
};
