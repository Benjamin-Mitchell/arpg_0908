// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "ArpgDynamicGameplayEffect.generated.h"

/**
 * 
 */
UCLASS()
class ARPG_0908_API UArpgDynamicGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()
	virtual bool IsSupportedForNetworking() const override { return true;};
	virtual bool IsNameStableForNetworking() const override { return true;};
};
