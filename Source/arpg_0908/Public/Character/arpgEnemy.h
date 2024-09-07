// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "Character/arpgCharacterBase.h"
#include "Interaction/HighlightInterface.h"
#include "arpgEnemy.generated.h"

/**
 * 
 */
UCLASS()
class ARPG_0908_API AarpgEnemy : public AarpgCharacterBase, public IHighlightInterface
{
	GENERATED_BODY()

public:
	AarpgEnemy();

	//Highlight Interface
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;


	//Combat Interface
	virtual int32 GetPlayerLevel() override;

protected:
	virtual void BeginPlay() override;

	virtual void InitAbilityActorInfo() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int32 Level = 1;
};
