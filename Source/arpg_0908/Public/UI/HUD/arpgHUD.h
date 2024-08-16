// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "arpgHUD.generated.h"

class UarpgUserWidget;
/**
 * 
 */
UCLASS()
class ARPG_0908_API AarpgHUD : public AHUD
{
	GENERATED_BODY()
public:

	UPROPERTY()
	TObjectPtr<UarpgUserWidget> OverlayWidget;

protected:
	virtual void BeginPlay() override;
private:

	UPROPERTY(EditAnywhere)
	TSubclassOf<UarpgUserWidget> OverlayWidgetClass;
};
