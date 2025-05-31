// Copyright Tinzie Games


#include "Player/arpgPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "ArpgGameplayTags.h"
#include "Interaction/HighlightInterface.h"

#include "EnhancedInputSubsystems.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/arpgAbilitySystemComponent.h"
#include "AbilitySystem/ArpgAbilitySystemLibrary.h"
#include "Character/arpgCharacter.h"
#include "Character/arpgEnemy.h"
#include "Game/ArpgGameModeBase.h"
#include "Game/ArpgGameState.h"
#include "Input/arpgInputComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interaction/CombatInterface.h"
#include "UI/Widget/ArpgTemporaryTextComponent.h"
#include "UI/Widget/DamageTextComponent.h"


AarpgPlayerController::AarpgPlayerController()
{
	bReplicates = true;

	//moveComponent = Cast<UCharacterMovementComponent>(GetCharacter()->GetMovementComponent());
}

void AarpgPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();


	if (isTargetting)
	{
		//Some abilities should block rotation for their duration
		if (GetASC() && !GetASC()->HasMatchingGameplayTag(FArpgGameplayTags::Get().Player_Block_Rotation))
		{
			FRotator TargetRotation = (TabTargetActor->GetActorLocation() - GetCharacter()->GetActorLocation()).Rotation();
			TargetRotation.Pitch = 0.0f;

			
			SetControlRotation(TargetRotation);
		}
	}
}

void AarpgPlayerController::ShowDamageNumber_Implementation(float DamageAmount, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit)
{
	if(IsValid(TargetCharacter) && DamageTextComponentClass && IsLocalController())
	{
		UDamageTextComponent* DamageText = NewObject<UDamageTextComponent>(TargetCharacter, DamageTextComponentClass);
		DamageText->RegisterComponent();

		//Attach to set location, immediately detach.
		DamageText->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

		DamageText->SetDamageText(DamageAmount, bBlockedHit, bCriticalHit);
	}
}

void AarpgPlayerController::CursorTrace()
{
	static bool BlockedLastFrame = false;
	
	//Some abilities flag that cursor trace should be blocked until they are complete
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FArpgGameplayTags::Get().Player_Block_CursorTrace))
	{
		//Some abilities flag that existing highlight should be ended when they begin
		if (GetASC()->HasMatchingGameplayTag(FArpgGameplayTags::Get().Player_EndExisting_Highlight))
		{
			if (ThisActorHighlighted) ThisActorHighlighted->UnHighlightActor();
			
			if (LastActorHighlighted) LastActorHighlighted->UnHighlightActor();
            LastActorHighlighted = nullptr;
		}
		
		ThisActorHighlighted = nullptr;
		BlockedLastFrame = true;
		return;
	}
	
	if (BlockedLastFrame)
	{
		if (ThisActorHighlighted) ThisActorHighlighted->UnHighlightActor();
		ThisActorHighlighted = nullptr;
		if (LastActorHighlighted) LastActorHighlighted->UnHighlightActor();
		LastActorHighlighted = nullptr;
	}
	BlockedLastFrame = false;
	
	GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit) return;

	
	LastActorHighlighted = ThisActorHighlighted;
	ThisActorHighlighted = Cast<IHighlightInterface>(CursorHit.GetActor());

	
	if (LastActorHighlighted == nullptr)
	{
		//New object to highlight
		if (ThisActorHighlighted != nullptr)
		{
			ThisActorHighlighted->HighlightActor();
		}
		//else both are null, do nothing
	}
	else
	{
		//Last is valid, current is not
		if (ThisActorHighlighted == nullptr)
		{
			LastActorHighlighted->UnHighlightActor();
		}
		else
		{
			//If they're not the same
			if (LastActorHighlighted != ThisActorHighlighted)
			{
				//Un-highlight old, highlight new
				LastActorHighlighted->UnHighlightActor();
				ThisActorHighlighted->HighlightActor();
			}
		}
	}
}

void AarpgPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	//Some abilities flag that input should be blocked until they are complete
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FArpgGameplayTags::Get().Player_Block_InputPressed))
	{
		return;
	}
	if(GetASC() == nullptr) return;
	GetASC()->AbilityInputTagPressed(InputTag);
}

void AarpgPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	//Some abilities flag that input should be blocked until they are complete
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FArpgGameplayTags::Get().Player_Block_InputReleased))
    {
    	return;
    }
	if(GetASC() == nullptr) return;
	GetASC()->AbilityInputTagReleased(InputTag);
}

void AarpgPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	//Some abilities flag that input should be blocked until they are complete
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FArpgGameplayTags::Get().Player_Block_InputHeld))
	{
		return;
	}
	if(GetASC() == nullptr) return;
	GetASC()->AbilityInputTagHeld(InputTag);
}

UarpgAbilitySystemComponent* AarpgPlayerController::GetASC()
{
	if(ArpgAbilitySystemComponent == nullptr)
	{
		ArpgAbilitySystemComponent = Cast<UarpgAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()));
	}

	return ArpgAbilitySystemComponent;
}

void AarpgPlayerController::SetTargetingEnabled(bool InEnabled)
{
	GetCharacter()->GetCharacterMovement()->bOrientRotationToMovement = !InEnabled;
	isTargetting = InEnabled;
}

void AarpgPlayerController::SetTarget(AActor* NewTarget)
{
	if (NewTarget == nullptr)
	{
		//disable VFX
		if (TabTargetActor != nullptr)
		{
			//Disable VFX
			if (AarpgEnemy* Enemy = Cast<AarpgEnemy>(TabTargetActor))
			{
				//trigger event for on enemy target
				Enemy->OnTargettedByPlayer.Broadcast(false);
			}
		}
		TabTargetActor = nullptr;
		SetTargetingEnabled(false);

		}
	else
	{
		//Disable VFX on old one
		if (AarpgEnemy* Enemy = Cast<AarpgEnemy>(TabTargetActor))
		{
			//trigger event for on enemy target
			Enemy->OnTargettedByPlayer.Broadcast(false);
		}
		
		TabTargetActor = NewTarget;
		SetTargetingEnabled(true);

		//Enable VFX
		if (AarpgEnemy* Enemy = Cast<AarpgEnemy>(TabTargetActor))
		{
			//trigger event for on enemy target
			Enemy->OnTargettedByPlayer.Broadcast(true);
		}
	}
}

AActor* AarpgPlayerController::FindNearestTarget(const TArray<AActor*> &ActorsToIgnore)
{
	TArray<AActor*> FoundActors;
	
	UArpgAbilitySystemLibrary::GetLivePlayersWithinSphereRadius(GetWorld(), FoundActors, ActorsToIgnore, TabInteractDistance, CursorHit.Location, TArray<FName>());
	if (FoundActors.Num() > 0)
	{
		AActor* ClosestActor = nullptr;
		float ClosestDistance;
		for(AActor* NearbyActor : FoundActors)
		{
			//We can currently only target enemies
			if (UArpgAbilitySystemLibrary::IsNotFriendBasedOnTag(GetCharacter(), NearbyActor))
			{
				float DistanceToCheck = FVector::Distance(NearbyActor->GetActorLocation(), CursorHit.Location); 
				if (ClosestActor == nullptr || DistanceToCheck < ClosestDistance)
				{
					ClosestActor = NearbyActor;
					ClosestDistance = DistanceToCheck;
				}
			}
			
		}

		return ClosestActor;
	}

	return nullptr;
}

void AarpgPlayerController::TabTarget(const struct FInputActionValue& InputActionValue)
{
	TArray<AActor*> ActorsToIgnore = {GetCharacter()};

	GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, CursorHit);

	AActor* NewTarget;
	AActor* HitActor = CursorHit.GetActor();
	if (HitActor->Implements<UCombatInterface>() && UArpgAbilitySystemLibrary::IsNotFriendBasedOnTag(GetCharacter(), HitActor))
	{
		NewTarget = HitActor;
	}
	else
	{
		 NewTarget = FindNearestTarget(ActorsToIgnore);
		
	}
	

	if (NewTarget == TabTargetActor)
	{
		SetTarget(nullptr);
	}
	else
	{
		//Set New Target if it exists (nullptr if no other possibilities)
		//TODO: If Nullptr, rotate to another one?
		SetTarget(NewTarget);
	}
}


void AarpgPlayerController::ServerReportCardVote_Implementation(int voteIndex)
{
	AArpgGameState* GameState = Cast<AArpgGameState>(GetWorld()->GetGameState());
	GameState->PassVoteToGameMode(voteIndex, this);
}

void AarpgPlayerController::BeginPlay()
{
	Super::BeginPlay();

	//assert input mapping context is already set
	check(arpgIMContext);

	//Subsystems are singletons. They're the system for the local controlled player.
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	if (Subsystem)
	{
		Subsystem->AddMappingContext(arpgIMContext, 0);
	}

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::CardinalCross;

	//General settings for input, especially when to hide cursor, what it can effect etc.
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);

	SetInputMode(InputModeData);
}

void AarpgPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("setupInputComponent!"));

	UarpgInputComponent* ArpgInputComponent = CastChecked<UarpgInputComponent>(InputComponent);

	//Execute the Move Function whenever a Move button is pressed in the Input Action.
	ArpgInputComponent->BindAction(moveAction, ETriggerEvent::Triggered, this, &AarpgPlayerController::Move);

	//Execute the Interact Function if the interact button is pressed in the Input Action.
	ArpgInputComponent->BindAction(interactAction, ETriggerEvent::Started, this, &AarpgPlayerController::Interact);

	//Execute the Interact Function if the interact button is pressed in the Input Action.
	ArpgInputComponent->BindAction(TabAction, ETriggerEvent::Started, this, &AarpgPlayerController::TabTarget);

	//Bind all other functions to their respective abilities, no need for custom functionality here.
	ArpgInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}

void AarpgPlayerController::Move(const FInputActionValue& InputActionValue)
{
	//Some abilities flag that input should be blocked until they are complete
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FArpgGameplayTags::Get().Player_Block_InputPressed))
	{
		return;
	}
	
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();

	//Doesn't work with targetControlRotation being set (we don't want movement to be afftected by Controller's understanding of rotation)
	//const FRotator Rotation = GetControlRotation();
	
	const FRotator Rotation = FRotator(0, 0, 0);
	const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);


	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	//only execute actions after the pawn exists
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y, false);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X, false);
	}
}
void AarpgPlayerController::Interact(const struct FInputActionValue& InputActionValue)
{
	if(ThisActorHighlighted != nullptr)
	{
		AActor* Passable = Cast<AActor>(ThisActorHighlighted);
		ServerInteract(Passable);
	}
}

void AarpgPlayerController::ServerInteract_Implementation(AActor* Interacted)
{
	IHighlightInterface* InteractInterface = Cast<IHighlightInterface>(Interacted);
	InteractInterface->Interact(this);
	
}

