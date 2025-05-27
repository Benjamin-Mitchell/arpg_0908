// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "ArpgPlayerStart.generated.h"

/**
 * 
 */
UCLASS()
class ARPG_0908_API AArpgPlayerStart : public APlayerStart
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	bool IsIntroStart = false;
	
};
