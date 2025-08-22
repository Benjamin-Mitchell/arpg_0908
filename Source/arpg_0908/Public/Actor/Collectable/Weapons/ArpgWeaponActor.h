// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "TemporaryWeaponComponent.h"
#include "GameFramework/Actor.h"
#include "Interaction/HighlightInterface.h"
#include "ArpgWeaponActor.generated.h"

class UTimelineComponent;
class UCapsuleComponent;
class UGameplayAbility;

UCLASS()
class ARPG_0908_API AArpgWeaponActor : public AActor, public IHighlightInterface
{
	GENERATED_BODY()
	
public:
	//Highlight Interface
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
	virtual void Interact(AarpgPlayerController* InteractingPlayer) override;
	// Sets default values for this actor's properties
	AArpgWeaponActor();

	//Reference to skeletal mesh to add to player.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<USkeletalMesh> WeaponMeshRef;

	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> GrantedAbilities;

	float GetCapsuleHalfHeight();
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Defaults")
	int HighlightVal = 252;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Defaults")
	int BaseHighlightVal = 0;

	bool bCollectable = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Defaults")
	bool bIsTemporary = false;

	UPROPERTY(EditAnywhere, Category = "Defaults")
	EExpirationCondition ExpirationCondition;
	
	UPROPERTY(EditAnywhere, Category = "Defaults")
	float ExpirationPeriod;

	UPROPERTY(EditAnywhere, Category = "Defaults")
	int ExpirationUses;
	
	//The actual mesh of the actor
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> WeaponActorSkeletalMesh;

	UPROPERTY(Category=WeaponActor, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UCapsuleComponent> CapsuleComponent;

	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Defalts")
	FRotator MeshRotationPerSecond = FRotator(0.0f, 90.0f, 00.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Defalts")
	FRotator InitialMeshRotationOffset = FRotator(0.0f, 0.0f, 15.0f);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Defalts")
	FRotator ActorRotationPerSecond = FRotator(0.0f, 45.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Defalts")
	float BobHeight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UTimelineComponent* BobTimeline;

	// Curve float reference
	UPROPERTY(EditAnywhere, Category = "Movement")
	UCurveFloat* BobCurve;

	// Timeline callback function
	UFUNCTION()
	void HandleBobProgress(float Value);

	// Initial Z position for reference
	float InitialZ;
};
