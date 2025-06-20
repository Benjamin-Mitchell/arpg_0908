// Copyright Tinzie Games


#include "AbilitySystem/AsyncTasks/ListenForGameplayAbilityEnded.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/Abilities/arpgGameplayAbility.h"

UListenForGameplayAbilityEnded* UListenForGameplayAbilityEnded::ListenForGameplayAbilityEnded(
	UAbilitySystemComponent* AbilitySystemComponent, TSubclassOf<UGameplayAbility> AbilityClass)
{
	if (!IsValid(AbilitySystemComponent))
	{
		return nullptr;
	}

	auto AbilitySpec = AbilitySystemComponent->FindAbilitySpecFromClass(AbilityClass);
	
	auto AbilityInstance = AbilitySpec->GetPrimaryInstance();
	
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
