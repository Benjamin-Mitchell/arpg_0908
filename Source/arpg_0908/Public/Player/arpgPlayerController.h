// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "arpgPlayerController.generated.h"


//forward declaration to avoid including header files.
class UInputMappingContext;

class UInputAction;

struct FInputActionValue;
/**
 * 
 */
UCLASS()
class ARPG_0908_API AarpgPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AarpgPlayerController();

protected:

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> arpgIMContext;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> moveAction;

	void Move(const struct FInputActionValue& InputActionValue);
};
