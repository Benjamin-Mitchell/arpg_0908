// Copyright Tinzie Games


#include "Actor/arpgHeadActor.h"

#include "Character/arpgCharacter.h"
#include "Player/arpgPlayerController.h"

// Sets default values
AarpgHeadActor::AarpgHeadActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	bNetLoadOnClient = true;

	ActorHeadSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>("ActorHeadSkeletalMesh");

	ActorHeadSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ActorHeadSkeletalMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	ActorHeadSkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	ActorHeadSkeletalMesh->SetRenderCustomDepth(true);
	ActorHeadSkeletalMesh->SetCustomDepthStencilValue(0);

	//Potentially required for enabling interact visual cues.... but what cues?
	bIsInteractable = true;
}

void AarpgHeadActor::HighlightActor()
{
	//Once collectable, we can show a UI for an input to press to activate
	bCollectable = true;
	
	ActorHeadSkeletalMesh->SetCustomDepthStencilValue(250);
}
void AarpgHeadActor::UnHighlightActor()
{
	bCollectable = false;
	
	ActorHeadSkeletalMesh->SetCustomDepthStencilValue(0);
}

void AarpgHeadActor::Interact(AarpgPlayerController* InteractingPlayer)
{
	//This should only be called on the server!
	check(HasAuthority());
	
	//ArpgCharacter has the SetMesh function, and also the methods for adding abilities.
	AarpgCharacter* PlayerCharacter = Cast<AarpgCharacter>(InteractingPlayer->GetCharacter());

	PlayerCharacter->SetHead(this);

	Destroy();
}

