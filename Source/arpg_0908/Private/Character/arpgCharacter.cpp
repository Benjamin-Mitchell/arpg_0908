// Copyright Tinzie Games


#include "Character/arpgCharacter.h"
#include "Player/arpgPlayerState.h"
#include "AbilitySystemComponent.h"
#include "ArpgGameplayTags.h"
#include "AbilitySystem/arpgAbilitySystemComponent.h"
#include "AbilitySystem/ArpgAbilitySystemLibrary.h"
#include "AbilitySystem/Abilities/arpgGameplayAbility.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "Actor/HeadData.h"
#include "Actor/Collectable/Weapons/WeaponData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Materials/MaterialExpressionOperator.h"
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

//Must only be called on the server
void AarpgCharacter::GrantEquippedAbilitiesOnSpawn()
{
	AarpgPlayerState* ArpgPlayerState = GetPlayerState<AarpgPlayerState>();
	
	int EquippedWeaponIndex = ArpgPlayerState->GetEquippedWeaponIndex();
	if (EquippedWeaponIndex >= 0)
	{
		const FWeaponInfo WeaponData = WeaponDatabase->GetWeaponInfo(EquippedWeaponIndex);
		AArpgWeaponActor* WeaponActorRef = WeaponData.WeaponReference->GetDefaultObject<AArpgWeaponActor>();
		AddCharacterAbilities(WeaponActorRef->GrantedAbilities);
		MulticastSetWeaponMesh(EquippedWeaponIndex);
	}
	
	int EquippedHeadIndex = ArpgPlayerState->GetEquippedHeadIndex();
	if (EquippedHeadIndex >= 0)
	{
		const FHeadInfo HeadData = HeadDatabase->GetHeadInfo(EquippedHeadIndex);
		AarpgHeadActor* HeadActorRef = HeadData.HeadReference->GetDefaultObject<AarpgHeadActor>();	
		AddCharacterAbilities(HeadActorRef->GrantedAbilities);
		MulticastSetHeadMesh(EquippedHeadIndex);
	}
}

void AarpgCharacter::AddInputScroll(float InScroll)
{
	ReceiveScrollInput(InScroll);	
}

// void AarpgCharacter::BeginPlay()
// {
// 	Super::BeginPlay();
//
// 	AarpgPlayerState* ArpgPlayerState = GetPlayerState<AarpgPlayerState>();
//
// 	
// 	if (ArpgPlayerState != nullptr)
// 	{
// 		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Character Begin Play. Weapon Index: %d"), ArpgPlayerState->GetEquippedWeaponIndex()));
// 		SetWeaponMesh(ArpgPlayerState->GetEquippedWeaponIndex());
// 		SetHeadMesh(ArpgPlayerState->GetEquippedHeadIndex());
// 	}
// }

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
	
	AarpgPlayerState* ArpgPlayerState = Cast<AarpgPlayerState>(GetPlayerState());
	if (ArpgPlayerState != nullptr)
	{
		ArpgPlayerState->SetEquippedHead(HeadIndex);
	}
	
	CurrentHeadActorClass = HeadData.HeadReference;
}

//This should only be called on the server!
void AarpgCharacter::SetWeapon(AArpgWeaponActor* NewWeaponActor, TemporaryWeaponComponent* TempWeaponComponent, bool IsTemporary)
{
	int WeaponIndex = WeaponDatabase->GetWeaponIndex(NewWeaponActor);
	FVector WeaponWorldLocation = NewWeaponActor->GetActorLocation();
	FVector FloorWorldLocation = UArpgAbilitySystemLibrary::GetFloorPositionBelowLocation(this, WeaponWorldLocation, 5.0f);

	
	const FWeaponInfo WeaponData = WeaponDatabase->GetWeaponInfo(WeaponIndex);
	AArpgWeaponActor* NewWeaponActorRef = WeaponData.WeaponReference->GetDefaultObject<AArpgWeaponActor>();

	if (IsTemporary)
	{
		//This is a pure delegate.
		//TempWeaponComponent.ConditionMet = FOnTempWeaponConditionMet::CreateUObject(this, &AarpgCharacter::TemporaryWeaponExpired);
		CurrentTempWeaponComponent = TempWeaponComponent;
		CurrentTempWeaponComponent->ConditionMet.BindUObject(this, &AarpgCharacter::TemporaryWeaponExpired);
			
		TempEquippedWeaponActorClass = WeaponData.WeaponReference;
	}
	
	
	if (CurrentWeaponActorClass != nullptr)
	{
		//Remove old head abilities, if any
		AArpgWeaponActor* OldWeaponActorRef = CurrentWeaponActorClass->GetDefaultObject<AArpgWeaponActor>();

		if (IsTemporary)
		{
			//TODO: Disable instead of removing? (Leave a grey-UI-version)
			//DisableCharacterAbilities(OldWeaponActorRef->GrantedAbilities);
			RemoveCharacterAbilities(OldWeaponActorRef->GrantedAbilities);
		}
		else
		{
			RemoveCharacterAbilities(OldWeaponActorRef->GrantedAbilities);
			
			//Now we replace the old one with the new one, before it is deleted.
			FRotator Rotation = (GetActorLocation() - WeaponWorldLocation).Rotation();
			Rotation.Pitch = 0.f;

			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = this;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
			FVector NewSpawnLocation = WeaponWorldLocation;

			//If the floor is closer than half our capsule height
			float NewWeaponCapsuleHalfHeight = NewWeaponActorRef->GetCapsuleHalfHeight();
			if (abs(FloorWorldLocation.Z - WeaponWorldLocation.Z) < NewWeaponCapsuleHalfHeight)
			{
				NewSpawnLocation.Z = FloorWorldLocation.Z + NewWeaponCapsuleHalfHeight;
			}

			AArpgWeaponActor* NewWeapon = GetWorld()->SpawnActor<AArpgWeaponActor>(
					CurrentWeaponActorClass,
					NewSpawnLocation,
					Rotation,
					SpawnParams);	

			
		}	
	}

	if (!IsTemporary)
	{
		CurrentWeaponActorClass = WeaponData.WeaponReference;

		//This saves the equipped weapon. We don't save temporary weapons.
		AarpgPlayerState* ArpgPlayerState = Cast<AarpgPlayerState>(GetPlayerState());
		if (ArpgPlayerState != nullptr)
		{
			ArpgPlayerState->SetEquippedWeapon(WeaponIndex);
		}
	}
	
	
	AddCharacterAbilities(NewWeaponActorRef->GrantedAbilities);

	MulticastSetWeaponMesh(WeaponIndex);
	
}

void AarpgCharacter::SetWeaponMesh(int WeaponIndex)
{
	if (WeaponIndex < 0)
	{
		Weapon->SetSkeletalMesh(nullptr);
		return;
	}
	
	const FWeaponInfo WeaponData = WeaponDatabase->GetWeaponInfo(WeaponIndex);
	AArpgWeaponActor* WeaponActorRef = WeaponData.WeaponReference->GetDefaultObject<AArpgWeaponActor>();
	
	Weapon->SetSkeletalMesh(WeaponActorRef->WeaponMeshRef);

	//We don't currently do custom attachments here since they are attached by default already.
	//We may need to do work here in the future if we want to accomodate dual-wielding or two-handed weapons.
	//Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
}

void AarpgCharacter::SetHeadMesh(int HeadIndex)
{
	if (HeadIndex < 0)
		return;
	
	const FHeadInfo HeadData = HeadDatabase->GetHeadInfo(HeadIndex);
	AarpgHeadActor* HeadActorRef = HeadData.HeadReference->GetDefaultObject<AarpgHeadActor>();
	
	BaseHeadMesh->SetSkeletalMesh(HeadActorRef->HeadMeshRef);
	BaseHeadMesh->SetupAttachment(GetMesh(), FName("HeadSocket"));
}

void AarpgCharacter::MulticastSetHeadMesh_Implementation(int HeadIndex)
{
	SetHeadMesh(HeadIndex);
}

void AarpgCharacter::MulticastSetWeaponMesh_Implementation(int WeaponIndex)
{
	SetWeaponMesh(WeaponIndex);

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

void AarpgCharacter::AbilitySuccessfullyCast(UGameplayAbility* ActivatedAbility)
{
	if (CurrentTempWeaponComponent != nullptr)
	{
		FGameplayTagQuery GameplayTagQuery = FGameplayTagQuery::MakeQuery_MatchAllTags(ActivatedAbility->AbilityTags);
		
		AArpgWeaponActor* TempWeaponActorRef = TempEquippedWeaponActorClass->GetDefaultObject<AArpgWeaponActor>();
		for (TSubclassOf<UGameplayAbility> Ability : TempWeaponActorRef->GrantedAbilities)
		{
			FGameplayTagContainer AbilityTags = Ability->GetDefaultObject<UGameplayAbility>()->AbilityTags; 
			if (AbilityTags.MatchesQuery(GameplayTagQuery))
			{
				//Passes if this is the _final_ use of this temporary weapon.
				if (CurrentTempWeaponComponent->RecordAbilityUsage())
				{
					//Now we just want to add a callback that will remove the temporary weapon after this activation is complete.
					UarpgGameplayAbility* ActiveArpgAbility = Cast<UarpgGameplayAbility>(ActivatedAbility);
					ActiveArpgAbility->OnAbilityEnded.AddDynamic(this, &AarpgCharacter::TemporaryWeaponExpired);	
				}
			}
		}
	}
}

void AarpgCharacter::TemporaryWeaponExpired()
{
	//Remove Temporary Abilities
	AArpgWeaponActor* EquippedWeaponRef = TempEquippedWeaponActorClass->GetDefaultObject<AArpgWeaponActor>();
	RemoveCharacterAbilities(EquippedWeaponRef->GrantedAbilities);

	//Re-add previous permanent weapon, if there was any.
	if (IsValid(CurrentWeaponActorClass))
	{
		AArpgWeaponActor* OldWeaponActorRef = CurrentWeaponActorClass->GetDefaultObject<AArpgWeaponActor>();
		AddCharacterAbilities(OldWeaponActorRef->GrantedAbilities);
		
		int WeaponIndex = WeaponDatabase->GetWeaponIndex(OldWeaponActorRef);
		MulticastSetWeaponMesh(WeaponIndex);
	}
	else
	{
		//Nullify
		MulticastSetWeaponMesh(-1);
	}


	//This is where we remove that loose pointer. There shouldn't be a case where this is null.
	delete CurrentTempWeaponComponent;
}

void AarpgCharacter::InitAbilityActorInfo()
{
	AarpgPlayerState* arpgPlayerState = GetPlayerState<AarpgPlayerState>();
	check(arpgPlayerState);

	arpgPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(arpgPlayerState, this);

	Cast<UarpgAbilitySystemComponent>(arpgPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();
	
	AbilitySystemComponent = arpgPlayerState->GetAbilitySystemComponent();

	//TODO: Why don't we just store a UarpgAbilitySystemComponent instead of a standard AbilitySystemComponent?
	UarpgAbilitySystemComponent* ArpgASC = CastChecked<UarpgAbilitySystemComponent>(AbilitySystemComponent);
	ArpgASC->AbilityActivatedCallbacks.AddUObject(this, &AarpgCharacter::AbilitySuccessfullyCast);
	
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
