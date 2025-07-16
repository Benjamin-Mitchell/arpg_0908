// Copyright Tinzie Games


#include "Character/arpgCharacter.h"
#include "Player/arpgPlayerState.h"
#include "AbilitySystemComponent.h"
#include "ArpgGameplayTags.h"
#include "AbilitySystem/arpgAbilitySystemComponent.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
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
}

void AarpgCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	//check(InitializedAlready == false);
	//Remote players are initialized a second time by the engine unfortunately.
	if (InitializedAlready) return;
	
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	
	//Init ability actor info for the server
	InitAbilityActorInfo();
	
	HandlePlayerHighlight();

	AddCharacterAbilities(StartupAbilities);
	InitializedAlready = true;
}

void AarpgCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	//Init ability actor info for the client
	InitAbilityActorInfo();
	HandlePlayerHighlight();
}

int32 AarpgCharacter::GetPlayerLevel()
{
	const AarpgPlayerState* arpgPlayerState = GetPlayerState<AarpgPlayerState>();
	return arpgPlayerState->GetPlayerLevel();
}

//This should only be called on the server!
void AarpgCharacter::SetHead(AarpgHeadActor* NewHeadActor)
{
	int HeadIndex = HeadDatabase->GetHeadIndex(NewHeadActor);
	FVector HeadWorldLocation = NewHeadActor->GetActorLocation();

	const FHeadInfo HeadData = HeadDatabase->GetHeadInfo(HeadIndex);
	AarpgHeadActor* NewHeadActorRef = HeadData.HeadReference->GetDefaultObject<AarpgHeadActor>();					

	if (CurrentHeadActorClass != nullptr)
	{
		//Remove old head abilities, if any
		AarpgHeadActor* OldHeadActorRef = CurrentHeadActorClass->GetDefaultObject<AarpgHeadActor>();
		RemoveCharacterAbilities(OldHeadActorRef->GrantedAbilities);
		
		//Now we replace the old one with the new one, before it is deleted.
		FRotator Rotation = (GetActorLocation() - HeadWorldLocation).Rotation();
		Rotation.Pitch = 0.f;

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
		AarpgHeadActor* NewHead = GetWorld()->SpawnActor<AarpgHeadActor>(
				CurrentHeadActorClass,
				HeadWorldLocation,
				Rotation,
				SpawnParams);
	}

	AddCharacterAbilities(NewHeadActorRef->GrantedAbilities);

	MulticastSetHeadMesh(HeadIndex);
	
	CurrentHeadActorClass = HeadData.HeadReference;
}

void AarpgCharacter::MulticastSetHeadMesh_Implementation(int HeadIndex)
{
	const FHeadInfo HeadData = HeadDatabase->GetHeadInfo(HeadIndex);
	AarpgHeadActor* HeadActorRef = HeadData.HeadReference->GetDefaultObject<AarpgHeadActor>();
	
	BaseHeadMesh->SetSkeletalMesh(HeadActorRef->HeadMeshRef);
	BaseHeadMesh->SetupAttachment(GetMesh(), FName("HeadSocket"));
}

void AarpgCharacter::Onrep_Stunned()
{
	if (UarpgAbilitySystemComponent* ArpgASC = Cast<UarpgAbilitySystemComponent>(AbilitySystemComponent))
	{
		const FArpgGameplayTags& GameplayTags = FArpgGameplayTags::Get();
		
		FGameplayTagContainer BlockedTags;
		BlockedTags.AddTag(GameplayTags.Player_Block_InputPressed);
		BlockedTags.AddTag(GameplayTags.Player_Block_InputHeld);
		BlockedTags.AddTag(GameplayTags.Player_Block_InputReleased);
		BlockedTags.AddTag(GameplayTags.Player_Block_Rotation);

		if (bIsStunned)
		{
			ArpgASC->AddLooseGameplayTags(BlockedTags);
			StunNiagaraComponent->Activate();
		}
		else
		{
			ArpgASC->RemoveLooseGameplayTags(BlockedTags);
			StunNiagaraComponent->Deactivate();
		}
	}
}

void AarpgCharacter::Onrep_Burned()
{
	if (bIsBurning)
	{
		BurnNiagaraComponent->Activate();
	}
	else
	{
		BurnNiagaraComponent->Deactivate();
	}
}

void AarpgCharacter::InitAbilityActorInfo()
{
	AarpgPlayerState* arpgPlayerState = GetPlayerState<AarpgPlayerState>();
	check(arpgPlayerState);

	arpgPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(arpgPlayerState, this);

	Cast<UarpgAbilitySystemComponent>(arpgPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();
	
	AbilitySystemComponent = arpgPlayerState->GetAbilitySystemComponent();
	AttributeSet = arpgPlayerState->GetAttributeSet();

	GetOnASCRegisteredDelegate().Broadcast(AbilitySystemComponent);

	AbilitySystemComponent->RegisterGameplayTagEvent(FArpgGameplayTags::Get().Debuff_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AarpgCharacter::StunTagChanged);

	//This is only valid on the owning client AND HOST
	if(AarpgPlayerController* arpgPlayerController = Cast<AarpgPlayerController>(GetController()))
	{
		if (arpgPlayerController->IsLocalController())
		{
			if( AarpgHUD* arpgHUD = Cast<AarpgHUD>(arpgPlayerController->GetHUD()))
			{
				arpgHUD->InitOverlay(arpgPlayerController, arpgPlayerState, AbilitySystemComponent, AttributeSet);
			}
		}
	}
	

	InitializeDefaultAttributes();
}

void AarpgCharacter::HandlePlayerHighlight()
{
	//This is only valid on the owning client AND HOST
	if(AarpgPlayerController* arpgPlayerController = Cast<AarpgPlayerController>(GetController()))
	{
		if (arpgPlayerController->IsLocalController())
		{
			GetMesh()->SetCustomDepthStencilValue(LocalMeshBaseHighlightVal);
			Weapon->SetCustomDepthStencilValue(LocalWeaponBaseHighlightVal);
			BaseHeadMesh->SetCustomDepthStencilValue(LocalHeadBaseHighlightVal);
			return;
		}
	}
	
	GetMesh()->SetCustomDepthStencilValue(RemoteMeshBaseHighlightVal);
	Weapon->SetCustomDepthStencilValue(RemoteWeaponBaseHighlightVal);
	BaseHeadMesh->SetCustomDepthStencilValue(RemoteHeadBaseHighlightVal);
	
}
