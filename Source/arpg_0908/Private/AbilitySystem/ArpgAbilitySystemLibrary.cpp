// Copyright Tinzie Games


#include "AbilitySystem/ArpgAbilitySystemLibrary.h"

#include "AbilitySystemComponent.h"
#include "ArpgAbilityTypes.h"
#include "Game/ArpgGameModeBase.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"

void UArpgAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC)
{
	AArpgGameModeBase* ArpgGameMode = Cast<AArpgGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));

	if(ArpgGameMode == nullptr)
		return;

	AActor* AvatarActor = ASC->GetAvatarActor();

	UCharacterClassInfo* ClassInfo = ArpgGameMode->CharacterClassInfo;

	FCharacterClassDefaultInfo ClassDefaultInfo = ClassInfo->GetClassDefaultInfo(CharacterClass);

	FGameplayEffectContextHandle PrimaryAttributesContextHandle = ASC->MakeEffectContext();
	PrimaryAttributesContextHandle.AddSourceObject(AvatarActor);
	FGameplayEffectSpecHandle PrimaryAttributesSpecHandle = ASC->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttributes, Level, PrimaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle DerivedAttributesContextHandle = ASC->MakeEffectContext();
	DerivedAttributesContextHandle.AddSourceObject(AvatarActor);
	FGameplayEffectSpecHandle DerivedAttributesSpecHandle = ASC->MakeOutgoingSpec(ClassInfo->DerivedAttributes, Level, DerivedAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*DerivedAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle VitalAttributesContextHandle = ASC->MakeEffectContext();
	VitalAttributesContextHandle.AddSourceObject(AvatarActor);
	FGameplayEffectSpecHandle VitalAttributesSpecHandle = ASC->MakeOutgoingSpec(ClassInfo->VitalAttributes, Level, VitalAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalAttributesSpecHandle.Data.Get());
}

void UArpgAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass)
{
	AArpgGameModeBase* ArpgGameMode = Cast<AArpgGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));

	if(ArpgGameMode == nullptr)
		return;

	UCharacterClassInfo* ClassInfo = ArpgGameMode->CharacterClassInfo;
	if(ClassInfo == nullptr) return;
	
	for(TSubclassOf<UGameplayAbility> AbilityClass : ClassInfo->CommonAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		ASC->GiveAbility(AbilitySpec);
	}


	const FCharacterClassDefaultInfo& DefaultInfo = ClassInfo->GetClassDefaultInfo(CharacterClass);
	
	for(TSubclassOf<UGameplayAbility> AbilityClass : DefaultInfo.Abilities)
	{
		ICombatInterface* CombatInterface = Cast<ICombatInterface>(ASC->GetAvatarActor());
		if(CombatInterface)
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, CombatInterface->GetPlayerLevel());
			ASC->GiveAbility(AbilitySpec);
		}
	}
}

bool UArpgAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if(const FArpgGameplayEffectContext* ArpgEffectContext = static_cast<const FArpgGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return ArpgEffectContext->IsBlockedHit();
	}

	return false;
}

bool UArpgAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if(const FArpgGameplayEffectContext* ArpgEffectContext = static_cast<const FArpgGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return ArpgEffectContext->IsCriticalHit();
	}

	return false;
}

void UArpgAbilitySystemLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle, bool InIsBlockedHit)
{
	if(FArpgGameplayEffectContext* ArpgEffectContext = static_cast<FArpgGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		ArpgEffectContext->SetIsBlockedHit(InIsBlockedHit);
	}
}

void UArpgAbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle,
	bool InIsCriticalHit)
{
	if(FArpgGameplayEffectContext* ArpgEffectContext = static_cast<FArpgGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		ArpgEffectContext->SetIsCriticalHit(InIsCriticalHit);
	}
}

void UArpgAbilitySystemLibrary::GetLivePlayersWithinRadius(const UObject* WorldContextObject,
	TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius,
	const FVector& SphereOrigin)
{
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore);
	
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		TArray<FOverlapResult> Overlaps;
		World->OverlapMultiByObjectType(Overlaps, SphereOrigin, FQuat::Identity, FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects), FCollisionShape::MakeSphere(Radius), SphereParams);
		for (FOverlapResult& Overlap : Overlaps)
		{
			if (Overlap.GetActor()->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(Overlap.GetActor()))
			{
				OutOverlappingActors.AddUnique(ICombatInterface::Execute_GetAvatar(Overlap.GetActor()));
			}
		}
	}
}

bool UArpgAbilitySystemLibrary::IsNotFriendBasedOnTag(AActor* FirstActor, AActor* SecondActor)
{
	const bool BothArePlayers = FirstActor->ActorHasTag(FName("Player")) && SecondActor->ActorHasTag(FName("Player"));
	const bool BothAreEnemies = FirstActor->ActorHasTag(FName("Enemy")) && SecondActor->ActorHasTag(FName("Enemy"));
	const bool Friends= BothArePlayers || BothAreEnemies;
	return !Friends;
}
