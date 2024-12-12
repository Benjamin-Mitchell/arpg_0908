// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "Character/arpgCharacterBase.h"
#include "arpgCharacter.generated.h"

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

	void SetHead(USkeletalMesh* NewHeadMesh, const TArray<TSubclassOf<UGameplayAbility>> &GrantedAbilities);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<USkeletalMeshComponent> BaseHeadMesh;

	
private:
	virtual void InitAbilityActorInfo() override;

	TArray<TSubclassOf<UGameplayAbility>> CurrentHeadAbilities;
};
