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
	
	//Map of potential Heads, each of which is an index, referencing a struct of Head data.
	//maybe should just be an array instead of a map?
	UPROPERTY(EditDefaultsOnly, Category="Heads")
	TArray<FHeadInfo> Heads;

	FHeadInfo GetHeadInfo(int Index);
	
	int GetHeadIndex(AarpgHeadActor* HeadActor);
};
