// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CardProgressData.generated.h"

class AarpgAIControlledCharacter;

USTRUCT(BlueprintType)
struct FCardProperties
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Card Properties")
	FString LevelName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Card Properties")
	TArray<TSubclassOf<AarpgAIControlledCharacter>> EnemiesToSpawn;
};

USTRUCT(BlueprintType)
struct FPerStageChoices
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category="Card Properties")
	TArray<FCardProperties> CardPropertiesPossibilities;
};




/**
 * 
 */
UCLASS()
class ARPG_0908_API UCardProgressData : public UDataAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditDefaultsOnly, Category="Default Game Cards")
	TArray<FPerStageChoices> CardPropertiesByStage;
};
