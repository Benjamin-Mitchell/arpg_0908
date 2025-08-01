// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "arpgHeadActor.h"
#include "Abilities/GameplayAbility.h"
#include "Engine/DataAsset.h"
#include "HeadData.generated.h"


USTRUCT(BlueprintType)
struct FHeadInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category="Test")
	TSubclassOf<AarpgHeadActor> HeadReference;
};
/**
 * 
 */
UCLASS()
class ARPG_0908_API UHeadData : public UDataAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditDefaultsOnly, Category="Heads")
	TArray<FHeadInfo> Heads;

	FHeadInfo GetHeadInfo(int Index);
	
	int GetHeadIndex(AarpgHeadActor* HeadActor);
};
