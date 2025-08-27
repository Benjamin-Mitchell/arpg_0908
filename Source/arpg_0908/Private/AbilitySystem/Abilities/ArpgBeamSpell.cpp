// Copyright Tinzie Games


#include "AbilitySystem/Abilities/ArpgBeamSpell.h"

#include "Character/arpgCharacterBase.h"

void UArpgBeamSpell::StoreMouseDataInfo(const FHitResult& hitResult, bool bSnapToCombatInterfaceChest, bool bFixZValue, float InZValue)
{
	if (hitResult.bBlockingHit)
	{
		MouseHitLocation = hitResult.ImpactPoint;

		ICombatInterface* HitCombatInterface = Cast<ICombatInterface>(hitResult.GetActor());
		
		if (HitCombatInterface && bSnapToCombatInterfaceChest)
		{
			AarpgCharacterBase* CharacterBase = Cast<AarpgCharacterBase>(ICombatInterface::Execute_GetAvatar(hitResult.GetActor()));
			MouseHitLocation = CharacterBase->GetMesh()->GetSocketLocation("UpperChestSocket");
		}

		if (bFixZValue)
		{
			MouseHitLocation.Z = InZValue;
		}

		OwnerCharacter->SetLastRecordedMouseLocation(MouseHitLocation);

		//If we hit the same thing as last time, don't re-do the bindings.
		if (hitResult.GetActor() == MouseHitActor)
			return;
		

		//Remove old binding if it exists and was a valid combat interface actor 
		if (ICombatInterface* PrevCombatInterface = Cast<ICombatInterface>(MouseHitActor))
		{
			if (PrevCombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &UArpgBeamSpell::PrimaryTargetDied))
			{
				PrevCombatInterface->GetOnDeathDelegate().RemoveDynamic(this, &UArpgBeamSpell::PrimaryTargetDied);
			}
		}

		//Now add new one if its a valid combat interface actor
		if (HitCombatInterface)
		{
			if (HitCombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &UArpgBeamSpell::PrimaryTargetDied))
			{
				HitCombatInterface->GetOnDeathDelegate().AddDynamic(this, &UArpgBeamSpell::PrimaryTargetDied);
			}
		}

		//Now update the variable for next time.
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
		OwnerCharacter = Cast<AarpgCharacter>(OwnerPlayerController->GetCharacter());
	}
}
