// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HighlightInterface.generated.h"

class AarpgPlayerController;
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHighlightInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ARPG_0908_API IHighlightInterface //TODO: rename this to InteractInterface?
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void HighlightActor() = 0;
	virtual void UnHighlightActor() = 0;
	virtual void Interact(AarpgPlayerController* InteractingPlayer) = 0;
};
