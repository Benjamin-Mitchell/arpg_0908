// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "Character/arpgCharacterBase.h"
#include "Interaction/HighlightInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/EnemyInterface.h"
#include "arpgEnemy.generated.h"

class UWidgetComponent;
class UGameplayAbility;
class UBehaviorTree;
class AArpgAIController;

/**
 * 
 */
UCLASS()
class ARPG_0908_API AarpgEnemy : public AarpgCharacterBase, public IHighlightInterface, public IEnemyInterface
{
	GENERATED_BODY()

public:
	AarpgEnemy();

	virtual void PossessedBy(AController* NewController) override;

	//Highlight Interface
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
	virtual void Interact(AarpgPlayerController* InteractingPlayer) override;


	//Combat Interface
	virtual int32 GetPlayerLevel() override;
	virtual void Die(const FVector& DeathImpulse) override;

	virtual void SetCombatTarget_Implementation(AActor* InCombatTarget) override;
	virtual AActor* GetCombatTarget_Implementation() const override;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;
	
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;

	void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bHitReacting = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float BaseWalkSpeed = 250.f;
	
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	float LifeSpan = 5.f;


	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	TObjectPtr<AActor> CombatTarget;	

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int BaseHighlightVal = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int WeaponBaseHighlightVal = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int HeadBaseHighlightVal = 0;

	virtual void InitAbilityActorInfo() override;
	virtual void InitializeDefaultAttributes() const override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int32 Level = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	ECharacterClass CharacterClass = ECharacterClass::NONE;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;

	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY()
	TObjectPtr<AArpgAIController> ArpgAIController;

	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> EnemyUtilityAbilities;
};
