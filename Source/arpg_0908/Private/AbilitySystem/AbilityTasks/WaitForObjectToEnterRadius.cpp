// Copyright Tinzie Games


#include "AbilitySystem/AbilityTasks/WaitForObjectToEnterRadius.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/ArpgAbilitySystemLibrary.h"

UWaitForObjectToEnterRadius* UWaitForObjectToEnterRadius::CreateWaitForObjectToEnterRadius(
	UGameplayAbility* OwningAbility, const FGameplayTagContainer& InTargetTags, float InRadius, FVector InBaseLocation, float InWaitSecondsPerScan)
{
	UWaitForObjectToEnterRadius* MyTask = NewAbilityTask<UWaitForObjectToEnterRadius>(OwningAbility);
	MyTask->InTargetTags = InTargetTags;
	MyTask->Radius = InRadius;
	MyTask->BaseLocation = InBaseLocation;
	MyTask->WaitSecondsPerScan = InWaitSecondsPerScan;

	return MyTask;
}

void UWaitForObjectToEnterRadius::Activate()
{
	//Scan immediately
	ScanTimer = WaitSecondsPerScan;
	
	bTickingTask = true;
	
	Caster = Ability->GetCurrentActorInfo()->AvatarActor;
	
	ActorsToIgnore.Add(Caster.Get());
}

void UWaitForObjectToEnterRadius::TickTask(float DeltaTime)
{
	ScanTimer += DeltaTime;

	if (ScanTimer >= WaitSecondsPerScan)
	{
		OverlappingActors.Empty();
		//perform a scan
		UArpgAbilitySystemLibrary::GetLivePlayersWithinSphereRadius(Caster.Get(), OverlappingActors, ActorsToIgnore, Radius, BaseLocation, IgnoreTags, false);

		TArray<TWeakObjectPtr<AActor>> ValidActors;
		for (AActor* OverlappedActor : OverlappingActors)
		{
			UAbilitySystemComponent* OverlappedASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OverlappedActor);
			if (OverlappedASC->HasAnyMatchingGameplayTags(InTargetTags))
			{
				ValidActors.Add(OverlappedActor);
			}
		}

		if (ValidActors.Num() > 0)
		{
			
			FGameplayAbilityTargetDataHandle DataHandle;
			FGameplayAbilityTargetData_ActorArray* ActorArray = new FGameplayAbilityTargetData_ActorArray();
			ActorArray->SetActors(ValidActors);
			DataHandle.Add(ActorArray);
			
			OnFoundActors.Broadcast(DataHandle);
		}
		ScanTimer = 0.0f;
	}
}

