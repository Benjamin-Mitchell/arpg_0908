// Copyright Tinzie Games


#include "AbilitySystem/AbilityTasks/DetectCollisionBetweenTrace.h"

UDetectCollisionBetweenTrace* UDetectCollisionBetweenTrace::CreateDetectCollisionBetweenTrace(
	USkeletalMeshComponent* InSkeletalMesh, int InIntermediateTraceCount, bool InShowDebugLines, UGameplayAbility* OwningAbility)
{
	UDetectCollisionBetweenTrace* MyObj = NewAbilityTask<UDetectCollisionBetweenTrace>(OwningAbility);
	MyObj->bTickingTask = true;
	MyObj->SkeletalMesh = InSkeletalMesh;
	MyObj->FirstRay = true; //Initially, it will shoot its first ray.
	MyObj->IntermediateTraceCount = InIntermediateTraceCount;
	MyObj->ShowDebugLines = InShowDebugLines;
	return MyObj;
}

void UDetectCollisionBetweenTrace::Activate()
{
	Super::Activate();
}

void UDetectCollisionBetweenTrace::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	FName StartTraceSocketName = FName("TraceBegin");
	FName EndTraceSocketName = FName("TraceEnd");
	FVector StartLocation = SkeletalMesh->GetSocketLocation(StartTraceSocketName);
	FVector EndLocation = SkeletalMesh->GetSocketLocation(EndTraceSocketName);

	if (ShowDebugLines)
		DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Yellow, false, 1, 0, 1);

	TArray<FHitResult> Hits;
	//TArray<AActor> ActorsToReturn;
	TArray<TWeakObjectPtr<AActor>> ActorsToReturn;

	//TODO: ECC_Pawn acceptable to just hit controlled characters?
	ECollisionChannel CollisionChannel = ECollisionChannel::ECC_Pawn;
	
	//First, the basic Ray
	if (GetWorld()->LineTraceMultiByChannel(Hits, StartLocation, EndLocation, CollisionChannel))
	{
		for (FHitResult Hit : Hits)
		{
			uint32 HitID = Hit.GetActor()->GetUniqueID();
			if (!UniqueObjectIDsHit.Contains(HitID))
			{
				UniqueObjectIDsHit.Add(HitID);

				//return this object.
				TWeakObjectPtr<AActor> HitActor = Hit.GetActor();
				ActorsToReturn.Add(HitActor);
			}
		}
	}

	//Intermediate Rays. These are used for very fast moving animations, to avoid missing targets in-between.
	//Higher IntermediateTraceCount means more intermediate rays.
	if (!FirstRay)
	{
		//if we want 1 intermediate trace, we need to divide the total size by 2 etc.
		float StepDivisor = static_cast<float>(IntermediateTraceCount + 1);
		FVector StartToStartStepSize = (StartLocation - PreviousStartLocation) / StepDivisor;
		FVector EndToEndStepSize = (EndLocation - PreviousEndLocation) / StepDivisor;

		if (ShowDebugLines)
		{
			DrawDebugLine(GetWorld(), StartLocation, PreviousStartLocation, FColor::Red, false, 1, 0, 1);
			DrawDebugLine(GetWorld(), EndLocation, PreviousEndLocation, FColor::Red, false, 1, 0, 1);
		}
			
		for (int i = 0; i < IntermediateTraceCount; i++)
		{
			FVector NewStartLocation = StartLocation - StartToStartStepSize * static_cast<float>((i + 1));
			FVector NewEndLocation = EndLocation - EndToEndStepSize * static_cast<float>((i + 1));

			if (ShowDebugLines)
			{
				DrawDebugLine(GetWorld(), NewStartLocation, NewEndLocation, FColor::Blue, false, 1, 0, 1);
			}
			
			if (GetWorld()->LineTraceMultiByChannel(Hits, NewStartLocation, NewEndLocation, CollisionChannel))
			{
				for (FHitResult Hit : Hits)
				{
					uint32 HitID = Hit.GetActor()->GetUniqueID();
					if (!UniqueObjectIDsHit.Contains(HitID))
					{
						UniqueObjectIDsHit.Add(HitID);
						
						//return this object.
						TWeakObjectPtr<AActor> HitActor = Hit.GetActor();
						ActorsToReturn.Add(HitActor);
					}
					
				}
			}
		}
	}

	if (ActorsToReturn.Num() > 0)
	{
		FGameplayAbilityTargetDataHandle DataHandle;
		FGameplayAbilityTargetData_ActorArray* ActorArray = new FGameplayAbilityTargetData_ActorArray();
		ActorArray->SetActors(ActorsToReturn);
		DataHandle.Add(ActorArray);

		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCollisionFound.Broadcast(DataHandle);
		}
	}

	PreviousStartLocation = StartLocation;
	PreviousEndLocation = EndLocation;
	FirstRay = false;
}
