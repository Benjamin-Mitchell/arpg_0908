// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ArpgGameModeBase.generated.h"

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
};
