// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "Actor/Collectable/Weapons/TemporaryWeaponComponent.h"
#include "Character/arpgCharacterBase.h"
#include "Interaction/HighlightInterface.h"
#include "arpgCharacter.generated.h"

struct FActiveGameplayEffectHandle;
class UWeaponData;
class AArpgWeaponActor;
class UCameraComponent;
class AarpgHeadActor;
class UHeadData;
/**
 * 
 */
UCLASS()
class ARPG_0908_API AarpgCharacter : public AarpgCharacterBase, public IHighlightInterface
{
	GENERATED_BODY()
	
public:
	AarpgCharacter();

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	//Highlight Interface
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
	virtual void Interact(AarpgPlayerController* InteractingPlayer) override;

	//Combat Interface
	virtual int32 GetPlayerLevel() override;

	void SetHead(AarpgHeadActor* HeadActor);
	void SetWeapon(AArpgWeaponActor* WeaponActor, TemporaryWeaponComponent* TempWeaponComponent, bool IsTemporary = false);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meme stuff")
	TArray<UAnimMontage*> DanceMontages;

	void GrantEquippedAbilitiesOnSpawn();

	void AddInputScroll(float InScroll);
protected:

	//virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int LocalMeshHighlightVal = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int LocalWeaponHighlightVal = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int LocalHeadHighlightVal = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int RemoteMeshHighlightVal = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int RemoteWeaponHighlightVal = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int RemoteHeadHighlightVal = 0;
	
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastSetHeadMesh(int headIndex);
	
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastSetWeaponMesh(int WeaponIndex, int BackWeaponIndex);

	virtual void Onrep_Stunned() override;

	virtual void Onrep_Burned() override;

	UFUNCTION(BlueprintImplementableEvent)
	void ReceiveScrollInput(float InScroll);
private:

	TemporaryWeaponComponent* CurrentTempWeaponComponent;
	void AbilitySuccessfullyCast(UGameplayAbility* ActivatedAbility);

	UFUNCTION()
	void TemporaryWeaponExpired();

	TArray<FActiveGameplayEffectHandle> ActiveDisableGameplayEffects;
	
	bool InitializedAlready = false;
	virtual void InitAbilityActorInfo() override;

	void SetWeaponMesh(int WeaponIndex, int BackWeaponIndex);
	void SetHeadMesh(int HeadIndex);

	void HandlePlayerHighlight(bool EnableHighlight);


	UPROPERTY(EditDefaultsOnly, Category = "EquippableData")
	TObjectPtr<UHeadData> HeadDatabase;

	UPROPERTY(EditDefaultsOnly, Category = "EquippableData")
	TObjectPtr<UWeaponData> WeaponDatabase;

	TSubclassOf<AarpgHeadActor> CurrentHeadActorClass;
	TSubclassOf<AArpgWeaponActor> CurrentWeaponActorClass;
	TSubclassOf<AArpgWeaponActor> TempEquippedWeaponActorClass;
	
	TArray<TSubclassOf<UGameplayAbility>> TemporarilyRemovedAbilities;
};
