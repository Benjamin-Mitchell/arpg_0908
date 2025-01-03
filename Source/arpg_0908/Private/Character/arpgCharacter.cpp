// Copyright Tinzie Games


#include "Character/arpgCharacter.h"
#include "Player/arpgPlayerState.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/arpgAbilitySystemComponent.h"
#include "Actor/HeadData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/arpgPlayerController.h"
#include "UI/HUD/arpgHUD.h"


AarpgCharacter::AarpgCharacter()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 400.0f, 0.0f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	
	BaseHeadMesh = CreateDefaultSubobject<USkeletalMeshComponent>("HeadMesh");
	BaseHeadMesh->SetupAttachment(GetMesh(), FName("HeadSocket"));
	BaseHeadMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AarpgCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	//Init ability actor info for the server
	InitAbilityActorInfo();

	AddCharacterAbilities(StartupAbilities);
}

void AarpgCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	//Init ability actor info for the client
	InitAbilityActorInfo();
}

int32 AarpgCharacter::GetPlayerLevel()
{
	const AarpgPlayerState* arpgPlayerState = GetPlayerState<AarpgPlayerState>();
	return arpgPlayerState->GetPlayerLevel();
}

void AarpgCharacter::SetHead(AarpgHeadActor* HeadActor)
{
	ServerSetHead(HeadDatabase->GetHeadIndex(HeadActor));
}

void AarpgCharacter::ServerSetHead_Implementation(int HeadIndex)
{
	const FHeadInfo HeadData = HeadDatabase->GetHeadInfo(HeadIndex);
	AarpgHeadActor* HeadActorRef = HeadData.HeadReference->GetDefaultObject<AarpgHeadActor>();
	if(CurrentHeadAbilities.Num() > 0)
	{
		RemoveCharacterAbilities(CurrentHeadAbilities);
	}
	AddCharacterAbilities(HeadActorRef->GrantedAbilities);
	CurrentHeadAbilities = HeadActorRef->GrantedAbilities;

	MulticastSetHeadMesh(HeadIndex);
}

void AarpgCharacter::MulticastSetHeadMesh_Implementation(int HeadIndex)
{
	const FHeadInfo HeadData = HeadDatabase->GetHeadInfo(HeadIndex);
	AarpgHeadActor* HeadActorRef = HeadData.HeadReference->GetDefaultObject<AarpgHeadActor>();
	
	BaseHeadMesh->SetSkeletalMesh(HeadActorRef->HeadMeshRef);
	BaseHeadMesh->SetupAttachment(GetMesh(), FName("HeadSocket"));
}

void AarpgCharacter::InitAbilityActorInfo()
{
	AarpgPlayerState* arpgPlayerState = GetPlayerState<AarpgPlayerState>();
	check(arpgPlayerState);

	arpgPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(arpgPlayerState, this);

	Cast<UarpgAbilitySystemComponent>(arpgPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();
	
	AbilitySystemComponent = arpgPlayerState->GetAbilitySystemComponent();
	AttributeSet = arpgPlayerState->GetAttributeSet();

	//This is only valid on the owning client 
	if(AarpgPlayerController* arpgPlayerController = Cast<AarpgPlayerController>(GetController()))
	{
		if(AarpgHUD* arpgHUD = Cast<AarpgHUD>(arpgPlayerController->GetHUD()))
		{
			arpgHUD->InitOverlay(arpgPlayerController, arpgPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}
	

	InitializeDefaultAttributes();
}
