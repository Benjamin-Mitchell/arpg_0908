// Copyright Tinzie Games


#include "AbilitySystem/Abilities/ArpgDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "ArpgGameplayTags.h"
#include "AbilitySystem/ArpgAbilitySystemLibrary.h"

void UArpgDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1);
	
	const float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, FArpgGameplayTags::Get().Damage, ScaledDamage);

	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
}

FDamageEffectParams UArpgDamageGameplayAbility::MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor) const
{
	FDamageEffectParams Params;

	Params.WorldContextObject = GetAvatarActorFromActorInfo();
	Params.DamageGameplayEffectClass = DamageEffectClass;
	Params.SourceAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	if (TargetActor) Params.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	Params.BaseDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	Params.BaseCritChance = BaseCritChanceMagnitude;
	Params.BaseBlockChanceReduction = BaseBlockChanceReductionMagnitude;
	Params.AbilityLevel = GetAbilityLevel();
	Params.DebuffTags = DebuffTags;
	Params.DeathImpulseMagnitude = DeathImpulseMagnitude;
	Params.KnockbackForceMagnitude = KnockbackForceMagnitude;
	Params.KnockbackChance = KnockbackChance;
	Params.KnockbackPitch = KnockbackPitch;

	//default forces
	if (IsValid(TargetActor))
	{
		const bool bKnockback = FMath::RandRange(1, 100) < KnockbackChance;

		if (bKnockback)
		{
			FRotator Rotation = (TargetActor->GetActorLocation() - GetAvatarActorFromActorInfo()->GetActorLocation()).Rotation();
			Rotation.Pitch = KnockbackPitch;
			const FVector ToTarget = Rotation.Vector();
			Params.DeathImpulse = ToTarget * DeathImpulseMagnitude;
			Params.KnockbackForce = ToTarget * KnockbackForceMagnitude;
		}
	}
	return Params;
}

float UArpgDamageGameplayAbility::GetDamageAtLevel() const
{
	return Damage.GetValueAtLevel(GetAbilityLevel());
}

FTaggedMontage UArpgDamageGameplayAbility::GetRandomTaggedMontageFromArray(
	const TArray<FTaggedMontage>& TaggedMontages) const
{
	if (TaggedMontages.Num() > 0)
	{
		const int32 Selection = FMath::RandRange(0, TaggedMontages.Num() - 1);
		return TaggedMontages[Selection];
	}
	return FTaggedMontage();
}

//Sets Rotation (Facing) and Movement (Traversal) Targets. Call before animation trigger
void UArpgDamageGameplayAbility::SetTargetsIfTargetExists(bool SnapToFloorBelowTarget, AActor* TargetActor, FRotator AdditionalRotation, bool AdditionalRotationAffectsTraverseTarget, bool DebugSphere)
{
	
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(AvatarActor);	

	FVector TargetLocation;
	
	if (IsValid(TargetActor))
	{
		TargetLocation = TargetActor->GetActorLocation();

		SetTargetsBasedOnInputLocation(SnapToFloorBelowTarget, TargetLocation, AdditionalRotation, AdditionalRotationAffectsTraverseTarget, DebugSphere);
	}
	else
	{		
		TargetLocation = AvatarActor->GetActorLocation();
		
		//Move the minimum traversal distance, in the direction we are facing already
		TargetLocation += AvatarActor->GetActorForwardVector() * TraversalDistanceMin;

		SetTargetsBasedOnInputLocation(SnapToFloorBelowTarget, TargetLocation, FRotator::ZeroRotator, false, DebugSphere);
	}
}

void UArpgDamageGameplayAbility::SetTargetsBasedOnInputLocation(bool SnapToFloorBelowTarget,
	FVector TraversalTargetLocation, FRotator AdditionalRotation, bool AdditionalRotationAffectsTraverseTarget, bool DebugSphere)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(AvatarActor);
	
	//Check length of avatar -> target, clamp the traversal target within min/max
	FVector ToTarget = TraversalTargetLocation - AvatarActor->GetActorLocation();
	float TraversalDistance = ToTarget.Length();
	TraversalDistance = FMath::Clamp(TraversalDistance, TraversalDistanceMin, TraversalDistanceMax);


	FVector RotatedToTarget = AdditionalRotation.RotateVector(ToTarget);
	FVector RotatedFaceTarget = RotatedToTarget + AvatarActor->GetActorLocation();
	CombatInterface->Execute_UpdateFacingTarget(AvatarActor, RotatedFaceTarget);

	FVector ToTargetOffset;
	if (AdditionalRotationAffectsTraverseTarget)
		ToTargetOffset = RotatedToTarget.GetSafeNormal() * TraversalDistance;
	else
		ToTargetOffset = ToTarget.GetSafeNormal() * TraversalDistance;
	
	TraversalTargetLocation = AvatarActor->GetActorLocation() + ToTargetOffset;
	
	FVector TraverseTarget;
	if (SnapToFloorBelowTarget)
		TraverseTarget = UArpgAbilitySystemLibrary::GetFloorPositionBelowLocation(AvatarActor, TraversalTargetLocation);
	else
		TraverseTarget = TraversalTargetLocation;

	CombatInterface->Execute_UpdateTraverseTarget(AvatarActor, TraverseTarget);

	if (DebugSphere)
	{
		const UWorld* World = GEngine->GetWorldFromContextObject(AvatarActor, EGetWorldErrorMode::LogAndReturnNull);
		DrawDebugSphere(World, TraverseTarget , 100, 30, FColor::Blue, false, 8.0f);
	}
}



