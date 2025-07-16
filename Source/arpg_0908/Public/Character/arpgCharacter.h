// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "Character/arpgCharacterBase.h"
#include "arpgCharacter.generated.h"

class UCameraComponent;
class AarpgHeadActor;
class UHeadData;
/**
 * 
 */
UCLASS()
class ARPG_0908_API AarpgCharacter : public AarpgCharacterBase
{
	GENERATED_BODY()
	
public:
	AarpgCharacter();

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	
	//Combat Interface
	virtual int32 GetPlayerLevel() override;

	void SetHead(AarpgHeadActor* HeadActor);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meme stuff")
	TArray<UAnimMontage*> DanceMontages;
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int LocalMeshBaseHighlightVal = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int LocalWeaponBaseHighlightVal = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int LocalHeadBaseHighlightVal = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int RemoteMeshBaseHighlightVal = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int RemoteWeaponBaseHighlightVal = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int RemoteHeadBaseHighlightVal = 0;
	
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastSetHeadMesh(int headIndex);

	virtual void Onrep_Stunned() override;

	virtual void Onrep_Burned() override;
private:

	
	bool InitializedAlready = false;
	virtual void InitAbilityActorInfo() override;

	void HandlePlayerHighlight();

	UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults")
	TObjectPtr<UHeadData> HeadDatabase;	

	TSubclassOf<AarpgHeadActor> CurrentHeadActorClass;
};
