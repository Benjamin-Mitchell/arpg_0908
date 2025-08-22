// Copyright Tinzie Games


#include "Actor/Collectable/Weapons/ArpgWeaponActor.h"

#include "Character/arpgCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/TimelineComponent.h"
#include "Player/arpgPlayerController.h"

// Sets default values
AArpgWeaponActor::AArpgWeaponActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bNetLoadOnClient = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>("Capsule");
	CapsuleComponent->InitCapsuleSize(34.0f, 88.0f);
	//SetRootComponent(CapsuleComponent);
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CapsuleComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CapsuleComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	CapsuleComponent->CanCharacterStepUpOn = ECB_No;
	CapsuleComponent->SetShouldUpdatePhysicsVolume(true);
	CapsuleComponent->SetCanEverAffectNavigation(false);
	CapsuleComponent->bDynamicObstacle = true;

	
	RootComponent = CapsuleComponent;
	
	WeaponActorSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponActorSkeletalMesh");
	WeaponActorSkeletalMesh->SetupAttachment(CapsuleComponent);
	
	WeaponActorSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponActorSkeletalMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	//WeaponActorSkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	WeaponActorSkeletalMesh->SetRenderCustomDepth(true);
	WeaponActorSkeletalMesh->SetCustomDepthStencilValue(BaseHighlightVal);

	// Create timeline component
	BobTimeline = CreateDefaultSubobject<UTimelineComponent>("BobTimeline");
	
	//Potentially required for enabling interact visual cues.... but what cues?
	bIsInteractable = true;
}

float AArpgWeaponActor::GetCapsuleHalfHeight()
{
	return CapsuleComponent->GetScaledCapsuleHalfHeight();
}

void AArpgWeaponActor::BeginPlay()
{
	Super::BeginPlay();
	
	// Store initial Z position
	InitialZ = GetActorLocation().Z;
	
	// Setup timeline if curve is assigned
	if (BobCurve)
	{
		// Bind the callback function
		FOnTimelineFloat ProgressFunction;
		ProgressFunction.BindUFunction(this, FName("HandleBobProgress"));
        
		// Add the float curve to the timeline
		BobTimeline->AddInterpFloat(BobCurve, ProgressFunction);
        
		// Set timeline to loop
		BobTimeline->SetLooping(true);
        
		// Play the timeline
		BobTimeline->PlayFromStart();
	}
	
	AddActorWorldRotation(InitialMeshRotationOffset);
}

void AArpgWeaponActor::Tick(float DeltaSeconds)
{
	WeaponActorSkeletalMesh->AddLocalRotation(MeshRotationPerSecond * DeltaSeconds);

	AddActorWorldRotation(ActorRotationPerSecond * DeltaSeconds);
}

void AArpgWeaponActor::HandleBobProgress(float Value)
{
	// Calculate new Z position based on curve value
	float NewZ = InitialZ + (Value * BobHeight);
    
	// Update actor location
	FVector NewLocation = GetActorLocation();
	NewLocation.Z = NewZ;
	SetActorLocation(NewLocation);
}

void AArpgWeaponActor::HighlightActor()
{
	//Once collectable, we can show a UI for an input to press to activate
	bCollectable = true;
	
	WeaponActorSkeletalMesh->SetCustomDepthStencilValue(HighlightVal);
}

void AArpgWeaponActor::UnHighlightActor()
{
	bCollectable = false;
	
	WeaponActorSkeletalMesh->SetCustomDepthStencilValue(BaseHighlightVal);
}

void AArpgWeaponActor::Interact(AarpgPlayerController* InteractingPlayer)
{
	//This should only be called on the server!
	check(HasAuthority());
	
	//ArpgCharacter has the SetMesh function, and also the methods for adding abilities.
	AarpgCharacter* PlayerCharacter = Cast<AarpgCharacter>(InteractingPlayer->GetCharacter());

	TemporaryWeaponComponent* TempWeaponComponent = nullptr;
	if (bIsTemporary)
	{
		//We use a pointer. This MUST be deleted when the temp component is no longer needed (in arpgCharacter, when Weapon is swapped out/destroyed)
		TempWeaponComponent = new TemporaryWeaponComponent;
		TempWeaponComponent->SetExpirationCondition(ExpirationCondition);
		TempWeaponComponent->SetTotalPeriod(ExpirationPeriod);
		TempWeaponComponent->SetTotalUses(ExpirationUses);
	}
	
	PlayerCharacter->SetWeapon(this, TempWeaponComponent, bIsTemporary);

	Destroy();
}

