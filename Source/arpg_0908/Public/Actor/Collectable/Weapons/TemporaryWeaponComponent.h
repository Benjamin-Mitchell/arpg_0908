// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"

DECLARE_DELEGATE(FOnTempWeaponConditionMet);

UENUM(BlueprintType)
enum class EExpirationCondition : uint8
{
	AfterNumUses,
	AfterPeriod
};

/**
 * 
 */
class ARPG_0908_API TemporaryWeaponComponent
{
public:
	TemporaryWeaponComponent();
	~TemporaryWeaponComponent();

	bool RecordAbilityUsage();
	void RecordWeaponTick(float DeltaTime);

	void SetTotalUses(int InTotalUses);
	void SetTotalPeriod(float InTotalPeriod);
	void SetExpirationCondition(EExpirationCondition Cond);

	FOnTempWeaponConditionMet ConditionMet;
private:
	

	int TotalUses;
	int TotalPeriod;

	float CurrentUses;
	float CurrentPeriod;

	EExpirationCondition ExpirationCondition;
	
	
};


