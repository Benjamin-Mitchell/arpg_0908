// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "ArpgAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class ARPG_0908_API UArpgAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	static UArpgAssetManager& Get();

protected:
	virtual void StartInitialLoading() override;
	
};
