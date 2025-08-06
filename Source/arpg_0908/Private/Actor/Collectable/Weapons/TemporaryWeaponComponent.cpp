// Copyright Tinzie Games


#include "Actor/Collectable/Weapons/TemporaryWeaponComponent.h"

TemporaryWeaponComponent::TemporaryWeaponComponent()
{
	CurrentUses = 0;
	TotalUses = 0;
	
	CurrentPeriod = 0.0f;
	TotalPeriod = 0.0f;
}

TemporaryWeaponComponent::~TemporaryWeaponComponent()
{
	
}

bool TemporaryWeaponComponent::RecordAbilityUsage()
{
	CurrentUses++;
	
	if (CurrentUses >= TotalUses && ExpirationCondition == EExpirationCondition::AfterNumUses)
	{
    	//ConditionMet.Execute();
		return true;
	}
	return false;
}


void TemporaryWeaponComponent::RecordWeaponTick(float DeltaTime)
{
	CurrentPeriod += DeltaTime;

	// if (CurrentPeriod >= TotalPeriod && ExpirationCondition == EExpirationCondition::AfterPeriod)
	// 	ConditionMet.Execute();
}

void TemporaryWeaponComponent::SetTotalUses(int InTotalUses)
{
	TotalUses = InTotalUses;	
}

void TemporaryWeaponComponent::SetTotalPeriod(float InTotalPeriod)
{
	TotalPeriod = InTotalPeriod;
}

void TemporaryWeaponComponent::SetExpirationCondition(EExpirationCondition Cond)
{
	ExpirationCondition = Cond;
}
