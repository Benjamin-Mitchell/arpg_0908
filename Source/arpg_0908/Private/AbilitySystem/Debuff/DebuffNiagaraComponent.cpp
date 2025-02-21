// Copyright Tinzie Games


#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilityBlueprint.h"
#include "Interaction/CombatInterface.h"

UDebuffNiagaraComponent::UDebuffNiagaraComponent()
{
	bAutoActivate = false;
}

void UDebuffNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();

	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner());
	
	if (ASC)
	{
		ASC->RegisterGameplayTagEvent(DebuffTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UDebuffNiagaraComponent::DebuffTagChanged);
	}
	else if (CombatInterface)
	{
		//if ASC is not available because this function is called too early, we assign a lambda to call the function when availability is broadcast by combat interface
		CombatInterface->GetOnASCRegisteredDelegate().AddWeakLambda(this, [this](UAbilitySystemComponent* InASC)
		{
			InASC->RegisterGameplayTagEvent(DebuffTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UDebuffNiagaraComponent::DebuffTagChanged);
		});
	}

	if (CombatInterface)
	{
		CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UDebuffNiagaraComponent::OnOwnerDeath);
	}
}

void UDebuffNiagaraComponent::DebuffTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount > 0)
	{
		Activate();
	}
	else
	{
		Deactivate();
	}
}

void UDebuffNiagaraComponent::OnOwnerDeath(AActor* DeadActor)
{
	Deactivate();
}
