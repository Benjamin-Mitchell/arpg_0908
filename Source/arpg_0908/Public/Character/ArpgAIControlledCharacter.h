// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "Character/arpgCharacterBase.h"
#include "Interaction/HighlightInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "Interaction/EnemyInterface.h"
#include "ArpgAIControlledCharacter.generated.h"

class UWidgetComponent;
class UGameplayAbility;
class UBehaviorTree;
class AArpgAIController;


//Must be defined multicast to allow blueprint assignable
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTargettedSignature, bool, NewValue);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTempDurationBeganSignature, float, Duration);


/**
 * 
 */
UCLASS()
class ARPG_0908_API AarpgAIControlledCharacter : public AarpgCharacterBase, public IHighlightInterface, public IEnemyInterface
{
	GENERATED_BODY()

public:
	AarpgAIControlledCharacter();

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

	UPROPERTY(BlueprintAssignable)
	FOnTargettedSignature OnTargettedByPlayer;

	void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bHitReacting = false;

	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	TObjectPtr<AActor> CombatTarget;

	UFUNCTION(BlueprintCallable)
	void StartBlackboard();

	UFUNCTION(BlueprintCallable)
	void GiveAndActivateSpawnAbility();

	UFUNCTION(BlueprintCallable)
	void GiveDespawnAbility();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
	UAnimMontage* SpawnAnimation;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
	UAnimMontage* DeSpawnAnimation;

	UPROPERTY(BlueprintAssignable)
	FOnTempDurationBeganSignature OnTempDurationBegan;
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	float PostDeathLifeSpan = 5.f;

	//A negative Temp Duration implies no lifespan 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float InitialTempDurationLifeSpan = -1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int BaseHighlightVal = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int WeaponBaseHighlightVal = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int HeadBaseHighlightVal = 0;

	virtual void InitAbilityActorInfo() override;
	virtual void InitializeDefaultAttributes() const override;
	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int32 Level = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> DurationBar;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> TargettedIcon;

	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY()
	TObjectPtr<AArpgAIController> ArpgAIController;

	UFUNCTION(BlueprintCallable)
	void SetBlackboardInAir(bool bInAir);
	
	UFUNCTION(BlueprintCallable)
	void SetBlackboardCustomBool(FName BoolName, bool Val);
	
	UFUNCTION(BlueprintCallable)
	void SetBlackboardCustomObject(FName KeyName, UObject* Obj);

	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> EnemyUtilityAbilities;

	UPROPERTY(EditAnywhere, Category = "Abilities")
	TSubclassOf<UGameplayAbility> SpawnAbility;

	UPROPERTY(EditAnywhere, Category = "Abilities")
	TSubclassOf<UGameplayAbility> DeSpawnAbility;

private:
	FTimerHandle TempDurationTimerHandle;

	void TriggerTempDurationOver();
};
