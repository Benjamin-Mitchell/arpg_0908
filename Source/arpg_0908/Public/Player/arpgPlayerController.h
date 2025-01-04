// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Interaction/HighlightInterface.h"
#include "arpgPlayerController.generated.h"


//forward declaration to avoid including header files.


class AarpgHeadActor;
class UDamageTextComponent;
struct FGameplayTag;
class UArpgInputConfig;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
//class IHighlightInterface;
class UarpgAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class ARPG_0908_API AarpgPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AarpgPlayerController();

	virtual void PlayerTick(float DeltaTime) override;

	UFUNCTION(Client, Reliable)
	void ShowDamageNumber(float DamageAmount, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit);

protected:

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> arpgIMContext;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> moveAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> interactAction;

	void Move(const struct FInputActionValue& InputActionValue);
	void Interact(const struct FInputActionValue& InputActionValue);

	UFUNCTION(Server, Reliable)
	void ServerInteract(AActor* Interacted);
	

	void CursorTrace();
	
	IHighlightInterface* LastActorHighlighted;
	IHighlightInterface* ThisActorHighlighted;

	FHitResult CursorHit;
	
	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);
	
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UArpgInputConfig> InputConfig;

	UPROPERTY()
	TObjectPtr<UarpgAbilitySystemComponent> ArpgAbilitySystemComponent;

	UarpgAbilitySystemComponent* GetASC();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTextComponent> DamageTextComponentClass;
};
