// Copyright Tinzie Games


#include "AbilitySystem/ArpgAbilitySystemLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "ArpgAbilityTypes.h"
#include "ArpgGameplayTags.h"
#include "AbilitySystem/Abilities/ArpgDamageGameplayAbility.h"
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

void UArpgAbilitySystemLibrary::GiveEnemyAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC,
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities, TArray<TSubclassOf<UGameplayAbility>> UtilityAbilities)
{
	
	for(TSubclassOf<UGameplayAbility> AbilityClass : UtilityAbilities)
	{
		ICombatInterface* CombatInterface = Cast<ICombatInterface>(ASC->GetAvatarActor());
		if(CombatInterface)
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, CombatInterface->GetPlayerLevel());
			ASC->GiveAbility(AbilitySpec);
		}
	}

	for(TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
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

bool UArpgAbilitySystemLibrary::IsSuccessfulDebuff(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if(const FArpgGameplayEffectContext* ArpgEffectContext = static_cast<const FArpgGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return ArpgEffectContext->IsSuccessfulDebuff();
	}

	return false;
}

float UArpgAbilitySystemLibrary::GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if(const FArpgGameplayEffectContext* ArpgEffectContext = static_cast<const FArpgGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return ArpgEffectContext->GetDebuffDamage();
	}

	return 0.f;
}

float UArpgAbilitySystemLibrary::GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if(const FArpgGameplayEffectContext* ArpgEffectContext = static_cast<const FArpgGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return ArpgEffectContext->GetDebuffDuration();
	}

	return 0.f;
}

float UArpgAbilitySystemLibrary::GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if(const FArpgGameplayEffectContext* ArpgEffectContext = static_cast<const FArpgGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return ArpgEffectContext->GetDebuffFrequency();
	}

	return 0.f;
}

FGameplayTag UArpgAbilitySystemLibrary::GetDebuffTag(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if(const FArpgGameplayEffectContext* ArpgEffectContext = static_cast<const FArpgGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		if (ArpgEffectContext->GetDebuffTag().IsValid())
		{
			return *ArpgEffectContext->GetDebuffTag();
		}
	}

	return FGameplayTag();
}

FVector UArpgAbilitySystemLibrary::GetDeathImpulse(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if(const FArpgGameplayEffectContext* ArpgEffectContext = static_cast<const FArpgGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return ArpgEffectContext->GetDeathImpulse();
	}

	return FVector::ZeroVector;
}

FVector UArpgAbilitySystemLibrary::GetKnockbackForce(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if(const FArpgGameplayEffectContext* ArpgEffectContext = static_cast<const FArpgGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return ArpgEffectContext->GetKnockbackForce();
	}

	return FVector::ZeroVector;
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

void UArpgAbilitySystemLibrary::SetIsSuccessfulDebuff(FGameplayEffectContextHandle& EffectContextHandle,
	bool InIsSuccessfulDebuff)
{
	if(FArpgGameplayEffectContext* ArpgEffectContext = static_cast<FArpgGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		ArpgEffectContext->SetIsSuccessfulDebuff(InIsSuccessfulDebuff);
	}
}

void UArpgAbilitySystemLibrary::SetDebuffDuration(FGameplayEffectContextHandle& EffectContextHandle,
	float InDebuffDuration)
{
	if(FArpgGameplayEffectContext* ArpgEffectContext = static_cast<FArpgGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		ArpgEffectContext->SetDebuffDuration(InDebuffDuration);
	}
}

void UArpgAbilitySystemLibrary::SetDebuffFrequency(FGameplayEffectContextHandle& EffectContextHandle,
	float InDebuffFrequency)
{
	if(FArpgGameplayEffectContext* ArpgEffectContext = static_cast<FArpgGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		ArpgEffectContext->SetDebuffFrequency(InDebuffFrequency);
	}
}

void UArpgAbilitySystemLibrary::SetDebuffDamage(FGameplayEffectContextHandle& EffectContextHandle, float InDebuffDamage)
{
	if(FArpgGameplayEffectContext* ArpgEffectContext = static_cast<FArpgGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		ArpgEffectContext->SetDebuffDamage(InDebuffDamage);
	}
}

void UArpgAbilitySystemLibrary::SetDebuffTag(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const FGameplayTag& InDebuffTag)
{
	if(FArpgGameplayEffectContext* ArpgEffectContext = static_cast<FArpgGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		TSharedPtr<FGameplayTag> DebuffTag = MakeShared<FGameplayTag>(InDebuffTag);
		ArpgEffectContext->SetDebuffTag(DebuffTag);
	}
}

void UArpgAbilitySystemLibrary::SetDeathImpulse(FGameplayEffectContextHandle& EffectContextHandle,
	const FVector& InDeathImpulse)
{
	if(FArpgGameplayEffectContext* ArpgEffectContext = static_cast<FArpgGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		ArpgEffectContext->SetDeathImpulse(InDeathImpulse);
	}
}

void UArpgAbilitySystemLibrary::SetKnockbackForce(FGameplayEffectContextHandle& EffectContextHandle,
	const FVector& InKnockbackForce)
{
	if(FArpgGameplayEffectContext* ArpgEffectContext = static_cast<FArpgGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		ArpgEffectContext->SetKnockbackForce(InKnockbackForce);
	}
}

void UArpgAbilitySystemLibrary::GetLivePlayersWithinSphereRadius(const UObject* WorldContextObject,
                                                           TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius,
                                                           const FVector& SphereOrigin, const TArray<FName>& IgnoreTags, bool DrawDebug)
{
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore);
	
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		TArray<FOverlapResult> Overlaps;
		World->OverlapMultiByObjectType(Overlaps, SphereOrigin, FQuat::Identity, FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects), FCollisionShape::MakeSphere(Radius), SphereParams);

		if (DrawDebug) DrawDebugSphere(World, SphereOrigin, Radius, 12, FColor::Green);
		
		for (FOverlapResult& Overlap : Overlaps)
		{
			if (Overlap.GetActor()->Implements<UCombatInterface>()
				&& !ICombatInterface::Execute_IsDead(Overlap.GetActor()))
			{
				bool ignore = false;
				
				for (FName TagToTest : IgnoreTags)
				{
					if (Overlap.GetActor()->ActorHasTag(TagToTest))
						ignore = true;
					
				}
				
				if (!ignore)
					OutOverlappingActors.AddUnique(ICombatInterface::Execute_GetAvatar(Overlap.GetActor()));
			}
		}
	}
}

void UArpgAbilitySystemLibrary::GetLivePlayersWithinCircleRadius(const UObject* WorldContextObject,
														   TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius,
														   const FVector& SphereOrigin, const TArray<FName>& IgnoreTags, bool DrawDebug)
{
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore);
	
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		TArray<FOverlapResult> Overlaps;
		World->OverlapMultiByObjectType(Overlaps, SphereOrigin, FQuat::Identity, FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects), FCollisionShape::MakeCapsule(Radius, 1.0f), SphereParams);

		if (DrawDebug) DrawDebugCapsule(World, SphereOrigin, 1.0f, Radius, FQuat::Identity, FColor::Green);
		
		for (FOverlapResult& Overlap : Overlaps)
		{
			if (Overlap.GetActor()->Implements<UCombatInterface>()
				&& !ICombatInterface::Execute_IsDead(Overlap.GetActor()))
			{
				bool ignore = false;
				
				for (FName TagToTest : IgnoreTags)
				{
					if (Overlap.GetActor()->ActorHasTag(TagToTest))
						ignore = true;
					
				}
				
				if (!ignore)
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

FGameplayEffectContextHandle UArpgAbilitySystemLibrary::ApplyDamageEffect(const FDamageEffectParams& Params)
{
	const FArpgGameplayTags& GameplayTags = FArpgGameplayTags::Get();
	const AActor* SourceAvatarActor = Params.SourceAbilitySystemComponent->GetAvatarActor();
	
	FGameplayEffectContextHandle EffectContextHandle = Params.SourceAbilitySystemComponent->MakeEffectContext();
	EffectContextHandle.AddSourceObject(SourceAvatarActor);
	SetDeathImpulse(EffectContextHandle, Params.DeathImpulse);
	SetKnockbackForce(EffectContextHandle, Params.KnockbackForce);
	const FGameplayEffectSpecHandle BaseDamageSpecHandle = Params.SourceAbilitySystemComponent->MakeOutgoingSpec(Params.DamageGameplayEffectClass, Params.AbilityLevel, EffectContextHandle);

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(BaseDamageSpecHandle, GameplayTags.Damage, Params.BaseDamage);
	Params.TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*BaseDamageSpecHandle.Data);


	//We are applying possibly many gameplay effects in one, is this a problem?
	for (FDebuffInfo DebuffInfo : Params.DebuffTags)
	{
		//Should this have a difference EffectClass?
		const FGameplayEffectSpecHandle DebuffSpecHandle = Params.SourceAbilitySystemComponent->MakeOutgoingSpec(Params.DamageGameplayEffectClass, Params.AbilityLevel, EffectContextHandle);
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DebuffSpecHandle, GameplayTags.Debuff_Damage, DebuffInfo.DebuffDamage);
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DebuffSpecHandle, GameplayTags.Debuff_Duration, DebuffInfo.DebuffDuration);
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DebuffSpecHandle, GameplayTags.Debuff_Frequency, DebuffInfo.DebuffFrequency);
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DebuffSpecHandle, GameplayTags.Debuff_Chance, DebuffInfo.DebuffChance);
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DebuffSpecHandle, DebuffInfo.DebuffTag, 1.0); //?
		Params.TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*DebuffSpecHandle.Data);
	}

	return EffectContextHandle;
	
}
