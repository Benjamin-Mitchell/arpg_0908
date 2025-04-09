// Copyright Tinzie Games


#include "AbilitySystem/AbilityTasks/WaitSynchronizeAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerState.h"

UWaitSynchronizeAbility* UWaitSynchronizeAbility::WaitForCheckPlayersWithTag(UGameplayAbility* OwningAbility,
                                                                             APawn* OwningActor, FGameplayTag RequiredTag, float Interval)
{
	UWaitSynchronizeAbility* MyObj = NewAbilityTask<UWaitSynchronizeAbility>(OwningAbility);
	MyObj->bTickingTask = true;
	MyObj->OwningActor = OwningActor;
	MyObj->CheckInterval = Interval;
	MyObj->SynchronizeTag = RequiredTag;
	return MyObj;
}

void UWaitSynchronizeAbility::Activate()
{
	APlayerState* PS = OwningActor->GetController<APlayerController>()->PlayerState;
	OwningPlayerIndex = PS->GetPlayerId();
	
	// Start checking immediately and then at intervals
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UWaitSynchronizeAbility::Check, CheckInterval, true);
}

void UWaitSynchronizeAbility::OnDestroy(bool AbilityEnded)
{
	Super::OnDestroy(AbilityEnded);
}

void UWaitSynchronizeAbility::BroadCastCasters()
{
	int32 LowerIndex = 0;
	int32 HigherIndex = 0;

	TArray<TWeakObjectPtr<AActor>> ActorsToReturn;
	
	TWeakObjectPtr<AActor> HighestActor;
	for (int i = 0; i < KnownCasters.Num(); i++)
	{
		APlayerState* PS = Cast<APawn>(KnownCasters[i])->GetController<APlayerController>()->PlayerState;
		int32 CasterPlayerID = PS->GetPlayerId();
		
		if (CasterPlayerID < OwningPlayerIndex && CasterPlayerID > LowerIndex)
		{
			ActorsToReturn.Add(KnownCasters[i]);
			LowerIndex = CasterPlayerID;
		}
		
		if (CasterPlayerID > HigherIndex)
		{
			HighestActor = KnownCasters[i];
			HigherIndex = CasterPlayerID;
		}
	}

	if (ActorsToReturn.Num() == 0 && KnownCasters.Num() > 1)
		ActorsToReturn.Add(HighestActor);

	FGameplayAbilityTargetDataHandle DataHandle;
	FGameplayAbilityTargetData_ActorArray* ActorArray = new FGameplayAbilityTargetData_ActorArray();
	ActorArray->SetActors(ActorsToReturn);
	DataHandle.Add(ActorArray);

	OnNewCaster.Broadcast(DataHandle);
}

TArray<AActor*> UWaitSynchronizeAbility::GetOtherActorsWithSynchronizeTag()
{
	TArray<AActor*> Actors;
	
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (APlayerController* PC = It->Get())
		{
			if (AActor* ControlledPawn = PC->GetPawn())
			{
				if (ControlledPawn != OwningActor)
				{
					UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(ControlledPawn);
					
					if (TargetASC->HasMatchingGameplayTag(SynchronizeTag))
					{
						Actors.Add(ControlledPawn);
					}
				}
			}
		}
	}
	
	return Actors;
}


void UWaitSynchronizeAbility::Check()
{
	TArray<AActor*> OtherCasters = GetOtherActorsWithSynchronizeTag();

	
	FColor Color = FColor::Green;
	if(IsLocallyControlled())
	{
		Color = FColor::Red;
	}

	//if theres a different number, broadcast immediately without needing to check the actual actors
	if (OtherCasters.Num() != KnownCasters.Num())
	{
		KnownCasters = OtherCasters;
		BroadCastCasters();
	}
	else
	{
		//Check they're actually the same
		for (int i = 0; i < OtherCasters.Num(); i++)
		{
			AActor* Caster = OtherCasters[i];
			if (Caster != KnownCasters[i])
			{
				KnownCasters = OtherCasters;
				BroadCastCasters();
				break;
			}
		}
	}
	
	KnownCasters = OtherCasters;
}