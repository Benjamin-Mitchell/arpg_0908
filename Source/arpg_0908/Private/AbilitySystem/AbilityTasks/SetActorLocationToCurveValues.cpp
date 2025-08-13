// Copyright Tinzie Games


#include "AbilitySystem/AbilityTasks/SetActorLocationToCurveValues.h"

USetActorLocationToCurveValues* USetActorLocationToCurveValues::MoveActorAlongMontageCurve(
	UGameplayAbility* OwningAbility, AActor* TargetActor, FName CurveNameX, FName CurveNameY, FName CurveNameZ,
	FVector BaseLocation, bool bUseActorStartAsBase)
{
	USetActorLocationToCurveValues* MyTask = NewAbilityTask<USetActorLocationToCurveValues>(OwningAbility);
	MyTask->ActorToMove = TargetActor;
	MyTask->CurveX = CurveNameX;
	MyTask->CurveY = CurveNameY;
	MyTask->CurveZ = CurveNameZ;
	MyTask->BaseLoc = BaseLocation;
	MyTask->bBaseFromActor = bUseActorStartAsBase;
	return MyTask;
}

void USetActorLocationToCurveValues::Activate()
{
	if (!ActorToMove)
	{
		EndTask();
		return;
	}

	if (bBaseFromActor)
	{
		BaseLoc = ActorToMove->GetActorLocation();
	}
	InitialActorLoc = BaseLoc;

	bTickingTask = true;
	//SetTickEnabled(true);
}

void USetActorLocationToCurveValues::TickTask(float DeltaTime)
{
	if (!ActorToMove)
	{
		EndTask();
		return;
	}

	// Get anim instance from Avatar actor
	USkeletalMeshComponent* MeshComp = Cast<USkeletalMeshComponent>(Ability->GetCurrentActorInfo()->AvatarActor->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	if (!MeshComp || !MeshComp->GetAnimInstance())
	{
		EndTask();
		return;
	}

	float OffsetX = CurveX.IsNone() ? 0.f : MeshComp->GetAnimInstance()->GetCurveValue(CurveX);
	float OffsetY = CurveY.IsNone() ? 0.f : MeshComp->GetAnimInstance()->GetCurveValue(CurveY);
	float OffsetZ = CurveZ.IsNone() ? 0.f : MeshComp->GetAnimInstance()->GetCurveValue(CurveZ);

	FVector NewLocation = InitialActorLoc + FVector(OffsetX, OffsetY, OffsetZ);
	ActorToMove->SetActorLocation(NewLocation);
}
