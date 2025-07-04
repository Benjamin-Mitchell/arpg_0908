#pragma once

#include "GameplayEffectTypes.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "ArpgAbilityTypes.generated.h"

class UGameplayEffect;



USTRUCT(BlueprintType)
struct FArpgGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

public:

	bool IsCriticalHit() const {return bIsCriticalHit;}
	bool IsBlockedHit() const {return bIsBlockedHit;}
	bool IsSuccessfulDebuff() const {return bIsSuccessfulDebuff;}
	float GetDebuffDuration() const {return DebuffDuration;}
	float GetDebuffFrequency() const {return DebuffFrequency;}
	float GetDebuffDamage() const {return DebuffDamage;}
	FVector GetDeathImpulse() const {return DeathImpulse;}
	FVector GetKnockbackForce() const {return KnockbackForce;}
	TSharedPtr<FGameplayTag> GetDebuffTag() const {return DebuffTag;}

	void SetIsCriticalHit(bool bInIsCriticalHit) { bIsCriticalHit = bInIsCriticalHit; }
	void SetIsBlockedHit(bool bInIsBlockedHit) {bIsBlockedHit = bInIsBlockedHit;}
	void SetIsSuccessfulDebuff(bool bInIsSuccessfulDebuff) {bIsSuccessfulDebuff = bInIsSuccessfulDebuff;}
	void SetDebuffDuration(float InDebuffDuration) {DebuffDuration = InDebuffDuration;}
	void SetDebuffFrequency(float InDebuffFrequency) {DebuffFrequency = InDebuffFrequency;}
	void SetDebuffDamage(float InDebuffDamage){DebuffDamage = InDebuffDamage;}
	void SetDebuffTag(TSharedPtr<FGameplayTag> InDebuffTag) {DebuffTag = InDebuffTag;}
	void SetDeathImpulse(const FVector& InImpulse) {DeathImpulse = InImpulse;}
	void SetKnockbackForce(const FVector& InForce) {KnockbackForce = InForce;}

	void SetTargetData(const FGameplayAbilityTargetDataHandle& Handle) { TargetData = Handle; }
	const FGameplayAbilityTargetDataHandle& GetTargetData() const { return TargetData; }
	
	
	
	/** Returns the actual struct used for serialization, subclasses must override this! */
	virtual UScriptStruct* GetScriptStruct() const
	{
		return StaticStruct();
	}

	/** Creates a copy of this context, used to duplicate for later modifications */
	virtual FArpgGameplayEffectContext* Duplicate() const
	{
		FArpgGameplayEffectContext* NewContext = new FArpgGameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		NewContext->TargetData = TargetData;
		return NewContext;
	}
	
	/** Custom serialization, subclasses must override this */
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
protected:

	UPROPERTY()
	FGameplayAbilityTargetDataHandle TargetData;
	
	UPROPERTY()
	bool bIsCriticalHit = false;

	UPROPERTY()
	bool bIsBlockedHit = false;

	UPROPERTY()
	bool bIsSuccessfulDebuff = false;

	UPROPERTY()
	float DebuffDuration = 0.f;
	UPROPERTY()
	float DebuffFrequency = 0.f;
	UPROPERTY()
	float DebuffDamage = 0.f;


	TSharedPtr<FGameplayTag> DebuffTag;

	UPROPERTY()
	FVector DeathImpulse = FVector::ZeroVector;
	
	
	UPROPERTY()
	FVector KnockbackForce = FVector::ZeroVector;
};

template<>
struct TStructOpsTypeTraits<FArpgGameplayEffectContext> : TStructOpsTypeTraitsBase2<FArpgGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true, 
		WithCopy = true
	};
};

///////////// TARGET DATA ///////////////////////////
USTRUCT(BlueprintType)
struct ARPG_0908_API FGameplayAbilityTargetData_CueInfo : public FGameplayAbilityTargetData
{
	GENERATED_BODY()
public:

	FGameplayAbilityTargetData_CueInfo()
	{ }

	UPROPERTY(BlueprintReadOnly)
	FVector OffsetVector = FVector(0.0f, 0.0f, 0.0f);

	UPROPERTY(BlueprintReadOnly)
	int Index = 0;

	// This is required for all child structs of FGameplayAbilityTargetData
	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FGameplayAbilityTargetData_CueInfo::StaticStruct();
	}

	// This is required for all child structs of FGameplayAbilityTargetData
	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		// The engine already defined NetSerialize for FName & FPredictionKey, thanks Epic!
		OffsetVector.NetSerialize(Ar, Map, bOutSuccess);
		Ar << Index; 
		bOutSuccess = true;
		return true;
	}
}; 

template<>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_CueInfo> : public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_CueInfo>
{
	enum
	{
		WithNetSerializer = true // This is REQUIRED for FGameplayAbilityTargetDataHandle net serialization to work
	};
};