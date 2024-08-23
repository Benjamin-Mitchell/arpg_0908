// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "Character/arpgCharacterBase.h"
#include "arpgCharacter.generated.h"

/**
 * 
 */
UCLASS()
class ARPG_0908_API AarpgCharacter : public AarpgCharacterBase
{
	GENERATED_BODY()
	
public:
	AarpgCharacter();

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

private:
	virtual void InitAbilityActorInfo() override;
};
