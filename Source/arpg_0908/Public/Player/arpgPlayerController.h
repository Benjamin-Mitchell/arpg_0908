// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Interaction/HighlightInterface.h"
#include "arpgPlayerController.generated.h"


//forward declaration to avoid including header files.
DECLARE_MULTICAST_DELEGATE(FOnPossesDelegate)


class AArpgPlayerCameraManager;
class ULevelSequence;
class UArpgTemporaryTextComponent;
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

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerReportCardVote(int voteIndex);

	UFUNCTION(BlueprintImplementableEvent)
	void RevertToNormalCamera(float BlendTime);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<AArpgPlayerCameraManager> ArpgPlayerCameraManager;

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void ClientSetCameraTransitionViaOffset(FVector TransitionOffset, FRotator InTargetRotator,
                                          float InTransitionDuration, bool InTransitionRotation);

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void ClientSetCameraTransitionToFrozen(FVector InTargetLocation, FRotator InTargetRotator,
									  float InTransitionDuration, bool InTransitionRotation, bool InTransitionZValue);

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void ClientSetCameraFrozen();
	
	FOnPossesDelegate OnPossesDelegate;
protected:

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	UPROPERTY(EditAnywhere)
	float CameraBlendtimeOnPossess = 5.0f;
private:
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> arpgIMContext;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> moveAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> ScrollAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> interactAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> TabAction;


	
	virtual void AcknowledgePossession(class APawn* P) override;
	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION(Server, Reliable)
	void ServerInteract(AActor* Interacted);
	
	void Move(const struct FInputActionValue& InputActionValue);
	void Scroll(const struct FInputActionValue& InputActionValue);
	void Interact(const struct FInputActionValue& InputActionValue);

	UFUNCTION(Server, Reliable)
	void ServerReportPossessionComplete();

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

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UArpgTemporaryTextComponent> SpeechTextComponentClass;

	//TObjectPtr<UCharacterMovementComponent> moveComponent;

	//Targetting Logic
	UPROPERTY(EditAnywhere, Category="TabTargetting")
	float TabInteractDistance = 100.0f;

	void SetTarget(AActor* NewTarget); 
	void TabTarget(const struct FInputActionValue& InputActionValue);
	AActor* FindNearestTarget(const TArray<AActor*>& ActorsToIgnore);
	AActor* TabTargetActor;
	void SetTargetingEnabled(bool InEnabled);
	bool isTargetting = false;
};
