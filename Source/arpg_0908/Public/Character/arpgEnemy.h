// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "Character/arpgCharacterBase.h"
#include "Interaction/HighlightInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "arpgEnemy.generated.h"

class UWidgetComponent;



/**
 * 
 */
UCLASS()
class ARPG_0908_API AarpgEnemy : public AarpgCharacterBase, public IHighlightInterface
{
	GENERATED_BODY()

public:
	AarpgEnemy();

	//Highlight Interface
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;


	//Combat Interface
	virtual int32 GetPlayerLevel() override;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;
	
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;

protected:
	virtual void BeginPlay() override;

	virtual void InitAbilityActorInfo() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int32 Level = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;
};
