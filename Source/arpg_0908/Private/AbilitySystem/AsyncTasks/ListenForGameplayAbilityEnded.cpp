// Copyright Tinzie Games


#include "AbilitySystem/AsyncTasks/ListenForGameplayAbilityEnded.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/Abilities/arpgGameplayAbility.h"
#include "arpg_0908/ArpgLogChannels.h"

UListenForGameplayAbilityEnded* UListenForGameplayAbilityEnded::ListenForGameplayAbilityEnded(
	UAbilitySystemComponent* AbilitySystemComponent, TSubclassOf<UGameplayAbility> AbilityClass)
{
	if (!IsValid(AbilitySystemComponent))
	{
		return nullptr;
	}

	auto AbilitySpec = AbilitySystemComponent->FindAbilitySpecFromClass(AbilityClass);

	auto AbilityInstance = AbilitySpec->GetPrimaryInstance();
	if (!IsValid(AbilityInstance))
	{
		UE_LOG(LogArpg, Error, TEXT("Ability Instance is NULL in ListenForGameplayAbilityEnded. This is probably because Ability is set to Instance per Execution, instead of per Actor. Only Instanced per Actor Abilities can be listened to for completion."));
	}
	
	UarpgGameplayAbility* ActiveAbility = Cast<UarpgGameplayAbility>(AbilityInstance);
	
	UListenForGameplayAbilityEnded* Task = NewObject<UListenForGameplayAbilityEnded>();
	ActiveAbility->OnAbilityEnded.AddDynamic(Task, &UListenForGameplayAbilityEnded::OnAbilityEndCallback);
	Task->AbilityListeningTo = ActiveAbility;
	
	return Task;
}

void UListenForGameplayAbilityEnded::OnAbilityEndCallback()
{
	OnAbilityEnd.Broadcast();
	
	AbilityListeningTo->OnAbilityEnded.RemoveDynamic(this, &UListenForGameplayAbilityEnded::OnAbilityEndCallback);	
}

void UListenForGameplayAbilityEnded::EndTask()
{
	if (AbilityListeningTo != nullptr)
		AbilityListeningTo->OnAbilityEnded.RemoveDynamic(this, &UListenForGameplayAbilityEnded::OnAbilityEndCallback);

	SetReadyToDestroy();
	MarkAsGarbage();
}
