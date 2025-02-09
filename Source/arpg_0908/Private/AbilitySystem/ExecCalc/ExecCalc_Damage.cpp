// Copyright Tinzie Games


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "ArpgAbilityTypes.h"
#include "ArpgGameplayTags.h"
#include "AbilitySystem/ArpgAbilitySystemLibrary.h"
#include "AbilitySystem/arpgAttributeSet.h"

struct ArpgDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageReduction);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);

	DECLARE_ATTRIBUTE_CAPTUREDEF(CritChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CritDamageMultiplier);
	
	ArpgDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UarpgAttributeSet, DamageReduction, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UarpgAttributeSet, BlockChance, Target, false);
		
		DEFINE_ATTRIBUTE_CAPTUREDEF(UarpgAttributeSet, CritChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UarpgAttributeSet, CritDamageMultiplier, Source, false);
	}
};

static const ArpgDamageStatics& DamageStatics()
{
	static ArpgDamageStatics DStatics;
	return DStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().DamageReductionDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	
	RelevantAttributesToCapture.Add(DamageStatics().CritChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CritDamageMultiplierDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	const AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	const AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	//Get Damage Set By Caller Magnitude
	float Damage = 0.f;
	const float DamageTypeValue = Spec.GetSetByCallerMagnitude(FArpgGameplayTags::Get().Damage);
	Damage += DamageTypeValue;

	float CritChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CritChanceDef, EvaluationParameters, CritChance);
	CritChance = FMath::Max<float>(0.f, CritChance);

	float CritDamageMultiplier = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CritDamageMultiplierDef, EvaluationParameters, CritDamageMultiplier);
	//CritDamageMultiplier = FMath::Max<float>(0.f, CritDamageMultiplier); //Crit Damage Multiplier Could be negative and can be greater than 100.0f.

	float CritRand = FMath::RandRange(0.0f, 100.f);
	 
	bool bCrit = CritRand < CritChance;

	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();
	UArpgAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, bCrit);
	
	if(bCrit)
	{
		float DefaultCritMultiplier = 1.5f;
		//Its a critical hit
		const float DamageMultiplier = DefaultCritMultiplier + (CritDamageMultiplier/100.f);
		Damage *= DamageMultiplier;
	}

	//Capture Block Chance On Target, and determine if there was a successful block
	//If Block, Halve the damage.
	float BlockChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluationParameters, BlockChance);
	BlockChance = FMath::Max<float>(0.f, BlockChance);

	//Capture Damage Reduction Percentage On Target, and reduce damage by that percentage
	float DamageReduction = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageReductionDef, EvaluationParameters, DamageReduction);
	DamageReduction = FMath::Max<float>(0.f, DamageReduction);

	float rand = FMath::RandRange(0.0f, 100.f);

	bool bBlocked = (rand < BlockChance);
	UArpgAbilitySystemLibrary::SetIsBlockedHit(EffectContextHandle, bBlocked);
	if(bBlocked)
	{
		Damage = Damage / 2.0f;
	}

	Damage -= (Damage * DamageReduction / 100.f);
	
	const FGameplayModifierEvaluatedData EvaluatedData(UarpgAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
	
}
