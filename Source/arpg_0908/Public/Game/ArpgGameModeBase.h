// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ArpgGameModeBase.generated.h"

class UHeadData;
class UCharacterClassInfo;
/**
 * 
 */
UCLASS()
class ARPG_0908_API AArpgGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults")
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;

	//Currently storing this on the character to keep it simple for repeated access.
	//If more things need access, move it here in the future.
	// UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults")
	// TObjectPtr<UHeadData> HeadDatabase;	
};
