// Copyright Tinzie Games


#include "Actor/arpgEffectActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

AarpgEffectActor::AarpgEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
}

void AarpgEffectActor::BeginPlay()
{
	Super::BeginPlay();
}

void AarpgEffectActor::ApplyEffectToTarget(AActor* TargetActor, FGameplayEffectAndPolicies GamePlayEffectClassAndPolicy)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if(TargetASC == nullptr) return;

	check(GamePlayEffectClassAndPolicy.GamePlayEffectClass);
	
	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GamePlayEffectClassAndPolicy.GamePlayEffectClass, 1.0f, EffectContextHandle);
	FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());

	const bool bIsInfinite = EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;

	if(bIsInfinite && GamePlayEffectClassAndPolicy.EffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		ActiveEffectHandles.Add(ActiveEffectHandle, TargetASC);
	}
	
}

void AarpgEffectActor::OnOverlap(AActor* TargetActor)
{
	for(FGameplayEffectAndPolicies& InstantEffectAndPolicy : InstantEffectAndPolicies)
	{
		if(InstantEffectAndPolicy.EffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
		{
			ApplyEffectToTarget(TargetActor, InstantEffectAndPolicy);
		}		
	}

	for(FGameplayEffectAndPolicies& DurationEffectAndPolicy : DurationEffectAndPolicies)
	{
		if(DurationEffectAndPolicy.EffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
		{
			ApplyEffectToTarget(TargetActor, DurationEffectAndPolicy);
		}		
	}

	for(FGameplayEffectAndPolicies& InfiniteEffectAndPolicy : InfiniteEffectAndPolicies)
	{
		if(InfiniteEffectAndPolicy.EffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
		{
			ApplyEffectToTarget(TargetActor, InfiniteEffectAndPolicy);
		}		
	}
}

void AarpgEffectActor::OnEndOverlap(AActor* TargetActor)
{

	for(FGameplayEffectAndPolicies& InstantEffectAndPolicy : InstantEffectAndPolicies)
	{
		if(InstantEffectAndPolicy.EffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
		{
			ApplyEffectToTarget(TargetActor, InstantEffectAndPolicy);
		}		
	}

	for(FGameplayEffectAndPolicies& DurationEffectAndPolicy : DurationEffectAndPolicies)
	{
		if(DurationEffectAndPolicy.EffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
		{
			ApplyEffectToTarget(TargetActor, DurationEffectAndPolicy);
		}		
	}

	for(FGameplayEffectAndPolicies& InfiniteEffectAndPolicy : InfiniteEffectAndPolicies)
	{
		if(InfiniteEffectAndPolicy.EffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
		{
			ApplyEffectToTarget(TargetActor, InfiniteEffectAndPolicy);
		}		
	}
	
	for(FGameplayEffectAndPolicies& InfiniteEffectAndPolicy : InfiniteEffectAndPolicies)
	{
		if(InfiniteEffectAndPolicy.EffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
		{
			UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
			if(!IsValid(TargetASC)) return;

			TArray<FActiveGameplayEffectHandle> HandlesToRemove;
			for(TTuple<FActiveGameplayEffectHandle, UAbilitySystemComponent*> HandlePair : ActiveEffectHandles)
			{
				if(TargetASC == HandlePair.Value)
				{
					TargetASC->RemoveActiveGameplayEffect(HandlePair.Key, 1);
					HandlesToRemove.Add(HandlePair.Key);
				}
			}

			for(FActiveGameplayEffectHandle& Handle : HandlesToRemove)
			{
				ActiveEffectHandles.FindAndRemoveChecked(Handle);
			}
		}		
	}
}

