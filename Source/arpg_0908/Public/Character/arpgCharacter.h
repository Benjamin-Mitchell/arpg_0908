// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "Character/arpgCharacterBase.h"
#include "arpgCharacter.generated.h"

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
	
protected:


	
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastSetHeadMesh(int headIndex);
private:
	virtual void InitAbilityActorInfo() override;

	UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults")
	TObjectPtr<UHeadData> HeadDatabase;	

	TSubclassOf<AarpgHeadActor> CurrentHeadActorClass;
};
