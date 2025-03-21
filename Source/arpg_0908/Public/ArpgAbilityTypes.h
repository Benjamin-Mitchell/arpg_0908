#pragma once

#include "GameplayEffectTypes.h"
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
		return NewContext;
	}
	
	/** Custom serialization, subclasses must override this */
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
protected:

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