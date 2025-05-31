// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "ArpgTemporaryTextComponent.generated.h"

/**
 * 
 */
UCLASS()
class ARPG_0908_API UArpgTemporaryTextComponent : public UWidgetComponent
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetTemporaryText(const FString& Text, float Duration);
	
};
