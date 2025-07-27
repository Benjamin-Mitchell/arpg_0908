// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SpeechStringDataSet.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class ARPG_0908_API USpeechStringDataSet : public UDataAsset
{
	GENERATED_BODY()
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Speech")
	TArray<FString> PlayerSpawnSpeechStrings;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Speech")
	TArray<FString> SpeechStrings;
};
