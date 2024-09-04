// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"

#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "arpgPlayerState.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;

UCLASS()
class ARPG_0908_API AarpgPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	AarpgPlayerState();

	//	Begin Ability System Interface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//	End Ability System Interface

	UAttributeSet* GetAttributeSet() const { return AttributeSet; };
protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

};
