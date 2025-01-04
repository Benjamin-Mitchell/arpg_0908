// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/HighlightInterface.h"
#include "arpgHeadActor.generated.h"

class UGameplayAbility;
class AarpgCharacter;

UCLASS()
class ARPG_0908_API AarpgHeadActor : public AActor, public IHighlightInterface
{
	GENERATED_BODY()
	
public:
	//Highlight Interface
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
	virtual void Interact(AarpgPlayerController* InteractingPlayer) override;
	
	// Sets default values for this actor's properties
	AarpgHeadActor();

	//Reference to skeletal mesh to add to player.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<USkeletalMesh> HeadMeshRef;

	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> GrantedAbilities;
protected:

	bool bCollectable = false;
	
	//The actual mesh of the actor
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Anything")
	TObjectPtr<USkeletalMeshComponent> ActorHeadSkeletalMesh;

};
