// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "arpgCharacterBase.generated.h"

UCLASS(Abstract)
class ARPG_0908_API AarpgCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AarpgCharacterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
