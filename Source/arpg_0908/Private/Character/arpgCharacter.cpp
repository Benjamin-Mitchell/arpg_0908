// Copyright Tinzie Games


#include "Character/arpgCharacter.h"
#include "Player/arpgPlayerState.h"
#include "AbilitySystemComponent.h"
#include "ArpgAbilityTypes.h"
#include "ArpgGameplayTags.h"
#include "InputBehavior.h"
#include "AbilitySystem/arpgAbilitySystemComponent.h"
#include "AbilitySystem/ArpgAbilitySystemLibrary.h"
#include "AbilitySystem/Abilities/arpgGameplayAbility.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "AbilitySystem/Utility/ArpgDynamicGameplayEffect.h"
#include "Actor/HeadData.h"
#include "Actor/Collectable/Weapons/WeaponData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"
#include "Materials/MaterialExpressionOperator.h"
#include "Net/UnrealNetwork.h"
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

void AarpgCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AarpgCharacter, LastRecordedMouseLocation);
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

	if (bHighlightOnSpawn)
		HandlePlayerHighlight(true);

	AddCharacterAbilities(StartupAbilities);
	
	InitializedAlready = true;
}

void AarpgCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	//Init ability actor info for the client
	InitAbilityActorInfo();
	
	if (bHighlightOnSpawn)
		HandlePlayerHighlight(true);
}

void AarpgCharacter::HighlightActor()
{
	HandlePlayerHighlight(true);
}

void AarpgCharacter::UnHighlightActor()
{
	HandlePlayerHighlight(false);
}

void AarpgCharacter::Interact(AarpgPlayerController* InteractingPlayer)
{
	//Currently does nothing.
	//Triggered on 'E' press. Could be a revive or something later down the line.
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
		MulticastSetWeaponMesh(EquippedWeaponIndex, -1);
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

FVector AarpgCharacter::GetLastRecordedMouseLocation()
{
	return LastRecordedMouseLocation;
}

void AarpgCharacter::SetLastRecordedMouseLocation(FVector InLocation)
{
	LastRecordedMouseLocation = InLocation;
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

	int BackWeaponIndex = -1;

	if (IsTemporary)
	{
		//TODO: Can this override another TempWeaponComponent pointer which is then leaked?
		//TODO: Clear old temporary weapon here if we still have one equipped
		CurrentTempWeaponComponent = TempWeaponComponent;
		TempEquippedWeaponActorClass = WeaponData.WeaponReference;
	}
	
	
	if (CurrentWeaponActorClass != nullptr)
	{
		//Remove old head abilities, if any
		AArpgWeaponActor* OldWeaponActorRef = CurrentWeaponActorClass->GetDefaultObject<AArpgWeaponActor>();


		if (IsTemporary)
		{
			AArpgWeaponActor* TempWeaponActorRef = TempEquippedWeaponActorClass->GetDefaultObject<AArpgWeaponActor>();
			TemporarilyRemovedAbilities.Empty();

			BackWeaponIndex = WeaponDatabase->GetWeaponIndex(OldWeaponActorRef);
			


			//Here we are looping through the abilities from the old weapon, and comparing their input tags to the abilities added by the temporary weapon.
			//If there are matches, we remove the current weapon's ability and add the old one.
			//If there are ability slots that don't match, we disable those with the appropriate Disable tag.
			for (TSubclassOf<UGameplayAbility> CurrentAbility : OldWeaponActorRef->GrantedAbilities)
			{
				TArray<FGameplayAbilitySpec*> CurrentMatchingAbilitySpecs;
				AbilitySystemComponent->GetActivatableGameplayAbilitySpecsByAllMatchingTags(CurrentAbility->GetDefaultObject<UGameplayAbility>()->GetAssetTags(), CurrentMatchingAbilitySpecs);

				//This is really only designed to support cases where there is a single ability with matching tag.
				FGameplayTag CurrentInputTag;
				for (FGameplayAbilitySpec* CurrentMatchingAbilitySpec : CurrentMatchingAbilitySpecs)
				{
					CurrentInputTag = UarpgAbilitySystemComponent::GetInputTagFromSpec(*CurrentMatchingAbilitySpec);
					if (CurrentInputTag.IsValid())
						break;
				}
				
				bool Found = false;
				for (TSubclassOf<UGameplayAbility> TempAbility : TempWeaponActorRef->GrantedAbilities)
				{
					//Directly taking the startupInputTag may be a problem if we allow some remapping functionality later? Although we shouldn't change the tags...
					UGameplayAbility* TempAbilityPtr = TempAbility->GetDefaultObject<UGameplayAbility>();
					UarpgGameplayAbility* ArpgAbility = Cast<UarpgGameplayAbility>(TempAbilityPtr);
					FGameplayTag TempInputTag = ArpgAbility->StartupInputTag;

					if (CurrentInputTag.MatchesTag(TempInputTag))
					{
						Found = true;
					}
				}

				if (Found)
					TemporarilyRemovedAbilities.Add(CurrentAbility);
				else
				{
					FGameplayTag DisableTag = FArpgGameplayTags::Get().InputToDisableTagMap[CurrentInputTag];
					
					//Unfortunately this doesn't actually trigger the delegate events for some reason.
					//AbilitySystemComponent->AddReplicatedLooseGameplayTag(DisableTag);

					//So we have to dynamically create a gameplay Effect to add this
					FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
					ContextHandle.AddSourceObject(this);
					
					UGameplayEffect* DisableEffect = NewObject<UArpgDynamicGameplayEffect>(GetTransientPackage(), FName(TEXT("DisableEffect")));
					
					DisableEffect->DurationPolicy = EGameplayEffectDurationType::Infinite;

					DisableEffect->StackingType = EGameplayEffectStackingType::AggregateBySource;
					DisableEffect->StackLimitCount = 1;
					
					FInheritedTagContainer TagContainer = FInheritedTagContainer();
					TagContainer.AddTag(DisableTag);

					UTargetTagsGameplayEffectComponent& Component = DisableEffect->FindOrAddComponent<UTargetTagsGameplayEffectComponent>();
					Component.SetAndApplyTargetTagChanges(TagContainer);					

					ContextHandle.AddSourceObject(this);
					FGameplayEffectSpec* MutableSpec = new FGameplayEffectSpec(DisableEffect, ContextHandle, 1.f);
					FArpgGameplayEffectContext* ArpgContext = static_cast<FArpgGameplayEffectContext*>(MutableSpec->GetContext().Get());
					
					ActiveDisableGameplayEffects.Add(AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*MutableSpec));
				}
			}
			
			//RemoveCharacterAbilities(OldWeaponActorRef->GrantedAbilities);
			RemoveCharacterAbilities(TemporarilyRemovedAbilities);
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
	
	MulticastSetWeaponMesh(WeaponIndex, BackWeaponIndex);
	
	AddCharacterAbilities(NewWeaponActorRef->GrantedAbilities);

	
}

void AarpgCharacter::SetWeaponMesh(int WeaponIndex, int BackWeaponIndex)
{
	if (WeaponIndex < 0)
	{
		Weapon->SetSkeletalMesh(nullptr);
	}
	else
	{
		const FWeaponInfo WeaponData = WeaponDatabase->GetWeaponInfo(WeaponIndex);
		AArpgWeaponActor* WeaponActorRef = WeaponData.WeaponReference->GetDefaultObject<AArpgWeaponActor>();
		
		Weapon->SetSkeletalMesh(WeaponActorRef->WeaponMeshRef);
	}
	
	
	if (BackWeaponIndex < 0)
	{
		StoredWeapon->SetSkeletalMesh(nullptr);
	}
	else
	{
		const FWeaponInfo WeaponData = WeaponDatabase->GetWeaponInfo(BackWeaponIndex);
		AArpgWeaponActor* WeaponActorRef = WeaponData.WeaponReference->GetDefaultObject<AArpgWeaponActor>();
		
		StoredWeapon->SetSkeletalMesh(WeaponActorRef->WeaponMeshRef);
	}

	

	

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

void AarpgCharacter::MulticastSetWeaponMesh_Implementation(int WeaponIndex, int BackWeaponIndex)
{
	SetWeaponMesh(WeaponIndex, BackWeaponIndex);

}

void AarpgCharacter::Onrep_Stunned()
{
	if (IsValid(AbilitySystemComponent))
	{
		const FArpgGameplayTags& GameplayTags = FArpgGameplayTags::Get();
		
		FGameplayTagContainer BlockedTags;
		BlockedTags.AddTag(GameplayTags.Player_Block_InputPressed);
		BlockedTags.AddTag(GameplayTags.Player_Block_InputHeld);
		BlockedTags.AddTag(GameplayTags.Player_Block_InputReleased);
		BlockedTags.AddTag(GameplayTags.Player_Block_Rotation);

		if (bIsStunned)
		{
			AbilitySystemComponent->AddLooseGameplayTags(BlockedTags);
			StunNiagaraComponent->Activate();
		}
		else
		{
			AbilitySystemComponent->RemoveLooseGameplayTags(BlockedTags);
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
		FGameplayTagQuery GameplayTagQuery = FGameplayTagQuery::MakeQuery_MatchAllTags(ActivatedAbility->GetAssetTags());
		
		AArpgWeaponActor* TempWeaponActorRef = TempEquippedWeaponActorClass->GetDefaultObject<AArpgWeaponActor>();
		for (TSubclassOf<UGameplayAbility> Ability : TempWeaponActorRef->GrantedAbilities)
		{
			FGameplayTagContainer AbilityTags = Ability->GetDefaultObject<UGameplayAbility>()->GetAssetTags(); 
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
	
	//Remove all the disability GEs
	for (FActiveGameplayEffectHandle ActiveDisable : ActiveDisableGameplayEffects)
	{
		AbilitySystemComponent->RemoveActiveGameplayEffect(ActiveDisable);
	}
	
	//Re-add previous permanent weapon, if there was any.
	if (IsValid(CurrentWeaponActorClass))
	{
		AArpgWeaponActor* OldWeaponActorRef = CurrentWeaponActorClass->GetDefaultObject<AArpgWeaponActor>();
	
		//AddCharacterAbilities(OldWeaponActorRef->GrantedAbilities);
		AddCharacterAbilities(TemporarilyRemovedAbilities);
	
		//TODO: Remove Disable Tags...
		
		
		int WeaponIndex = WeaponDatabase->GetWeaponIndex(OldWeaponActorRef);
		MulticastSetWeaponMesh(WeaponIndex, -1);
	}
	else
	{
		//Nullify
		MulticastSetWeaponMesh(-1, -1);
	}
	
	
	//This is where we remove that loose pointer. There shouldn't be a case where this is null.
	delete CurrentTempWeaponComponent;
}

void AarpgCharacter::InitAbilityActorInfo()
{
	AarpgPlayerState* arpgPlayerState = GetPlayerState<AarpgPlayerState>();
	check(arpgPlayerState);

	arpgPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(arpgPlayerState, this);

	AbilitySystemComponent = Cast<UarpgAbilitySystemComponent>(arpgPlayerState->GetAbilitySystemComponent()); 
	AbilitySystemComponent->AbilityActorInfoSet();

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


void AarpgCharacter::HandlePlayerHighlight(bool EnableHighlight)
{
	if (EnableHighlight)
	{
		//This is only valid on the owning client AND HOST
		if(AarpgPlayerController* arpgPlayerController = Cast<AarpgPlayerController>(GetController()))
		{
			if (arpgPlayerController->IsLocalController())
			{
				GetMesh()->SetCustomDepthStencilValue(LocalMeshHighlightVal);
				Weapon->SetCustomDepthStencilValue(LocalWeaponHighlightVal);
				BaseHeadMesh->SetCustomDepthStencilValue(LocalHeadHighlightVal);
				return;
			}
		}
	
		GetMesh()->SetCustomDepthStencilValue(RemoteMeshHighlightVal);
		Weapon->SetCustomDepthStencilValue(RemoteWeaponHighlightVal);
		BaseHeadMesh->SetCustomDepthStencilValue(RemoteHeadHighlightVal);
	}
	else
	{
		GetMesh()->SetCustomDepthStencilValue(0);
		Weapon->SetCustomDepthStencilValue(0);
		BaseHeadMesh->SetCustomDepthStencilValue(0);
	}
	
	
}
