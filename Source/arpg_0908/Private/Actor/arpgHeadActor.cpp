// Copyright Tinzie Games


#include "Actor/arpgHeadActor.h"

#include "Character/arpgCharacter.h"
#include "Player/arpgPlayerController.h"

// Sets default values
AarpgHeadActor::AarpgHeadActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ActorHeadMesh = CreateDefaultSubobject<UStaticMeshComponent>("ActorHeadMesh");
	
	//ActorHeadMesh = FindComponentByClass<UMeshComponent>();

	ActorHeadMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	ActorHeadMesh->SetRenderCustomDepth(true);
	ActorHeadMesh->SetCustomDepthStencilValue(0);

	//Potentially required for enabling interact visual cues.... but what cues?
	bIsInteractable = true;
}

void AarpgHeadActor::HighlightActor()
{
	//Once collectable, we can show a UI for an input to press to activate
	bCollectable = true;	

	
	ActorHeadMesh->SetCustomDepthStencilValue(250);
}
void AarpgHeadActor::UnHighlightActor()
{
	bCollectable = false;
	ActorHeadMesh->SetCustomDepthStencilValue(0);
}

void AarpgHeadActor::Interact(AarpgPlayerController* InteractingPlayer)
{
	//Now we finally need access to an ArpgCharacter (PlayerController doesn't have a reference to this, so we can't solve that.)

	//ArpgCharacter has the SetMesh function, and also the methods for adding abilities.
	AarpgCharacter* PlayerCharacter = Cast<AarpgCharacter>(InteractingPlayer->GetCharacter());

	PlayerCharacter->SetHead(HeadMeshRef, GrantedAbilities);
}

