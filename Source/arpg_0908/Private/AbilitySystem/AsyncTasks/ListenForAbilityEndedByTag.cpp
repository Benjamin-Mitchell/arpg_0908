// Copyright Tinzie Games


#include "AbilitySystem/AsyncTasks/ListenForAbilityEndedByTag.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/Abilities/arpgGameplayAbility.h"
#include "arpg_0908/ArpgLogChannels.h"


UListenForAbilityEndedByTag* UListenForAbilityEndedByTag::ListenForAbilityEndedByTag(
	UAbilitySystemComponent* AbilitySystemComponent, FGameplayTagContainer Tag)
{
	if (!IsValid(AbilitySystemComponent))
	{
		return nullptr;
	}

	
	TArray<FGameplayAbilitySpecHandle> GameplayAbilitySpecHandles;
	AbilitySystemComponent->FindAllAbilitiesWithTags(GameplayAbilitySpecHandles, Tag, true);
	auto AbilitySpec = AbilitySystemComponent->FindAbilitySpecFromHandle(GameplayAbilitySpecHandles[0]);

	auto AbilityInstance = AbilitySpec->GetPrimaryInstance();
	if (!IsValid(AbilityInstance))
	{
		UE_LOG(LogArpg, Error, TEXT("Ability Instance is NULL in ListenForGameplayAbilityEnded. This is probably because Ability is set to Instance per Execution, instead of per Actor. Only Instanced per Actor Abilities can be listened to for completion."));
	}
	
	UarpgGameplayAbility* ActiveAbility = Cast<UarpgGameplayAbility>(AbilityInstance);
	
	UListenForAbilityEndedByTag* Task = NewObject<UListenForAbilityEndedByTag>();
	ActiveAbility->OnAbilityEnded.AddDynamic(Task, &UListenForAbilityEndedByTag::OnAbilityEndCallback);
	Task->AbilityListeningTo = ActiveAbility;

	if (ActiveAbility->GetEndedAlready())
		return nullptr;
		
	return Task;
}

void UListenForAbilityEndedByTag::OnAbilityEndCallback()
{
	OnAbilityEnd.Broadcast();
	
	AbilityListeningTo->OnAbilityEnded.RemoveDynamic(this, &UListenForAbilityEndedByTag::OnAbilityEndCallback);	
}

void UListenForAbilityEndedByTag::EndTask()
{
	if (AbilityListeningTo != nullptr)
		AbilityListeningTo->OnAbilityEnded.RemoveDynamic(this, &UListenForAbilityEndedByTag::OnAbilityEndCallback);

	SetReadyToDestroy();
	MarkAsGarbage();
}
