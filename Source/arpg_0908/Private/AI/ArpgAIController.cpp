// Copyright Tinzie Games


#include "AI/ArpgAIController.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

AArpgAIController::AArpgAIController()
{
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>("BlackboardComponent");
	check(Blackboard);

	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>("BehaviorTreeComponent");
	check(BehaviorTreeComponent);
	
}

void AArpgAIController::CustomTeleportToLocation(const FVector& NewLocation)
{
	if (APawn* ControlledPawn = GetPawn())
	{
		// Stop active movement
		StopMovement();

		// Teleport with collision checks
		ControlledPawn->SetActorLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);
	}
}
