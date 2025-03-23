// Copyright Tinzie Games


#include "AbilitySystem/Abilities/ArpgBeamSpell.h"

void UArpgBeamSpell::StoreMouseDataInfo(const FHitResult& hitResult)
{
	if (hitResult.bBlockingHit)
	{
		MouseHitLocation = hitResult.ImpactPoint;
		MouseHitActor = hitResult.GetActor();
	}
	else
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}

void UArpgBeamSpell::StoreOwnerVariables()
{
	if (CurrentActorInfo)
	{
		OwnerPlayerController = CurrentActorInfo->PlayerController.Get();
		OwnerCharacter = OwnerPlayerController->GetCharacter();
	}
}
