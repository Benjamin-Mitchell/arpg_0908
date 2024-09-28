// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ArpgAIController.generated.h"


class UBlackboardComponent;
class UBehaviorTreeComponent;
/**
 * 
 */
UCLASS()
class ARPG_0908_API AArpgAIController : public AAIController
{
	GENERATED_BODY()

public:
	AArpgAIController();
	


protected:

	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;
};
