// Copyright Tinzie Games


#include "Player/arpgPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Interaction/HighlightInterface.h"

#include "EnhancedInputSubsystems.h"
#include "GameplayTagContainer.h"
#include "MovieSceneTracksComponentTypes.h"
#include "AbilitySystem/arpgAbilitySystemComponent.h"
#include "Input/arpgInputComponent.h"
#include "GameFramework/Character.h"
#include "UI/Widget/DamageTextComponent.h"


AarpgPlayerController::AarpgPlayerController()
{
	bReplicates = true;
}

void AarpgPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
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
			//If theyre not the same
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
	
}

void AarpgPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if(GetASC() == nullptr) return;
	GetASC()->AbilityInputTagReleased(InputTag);
}

void AarpgPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
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

	//Execute the Move Function whenever a Moove button is pressed in the Input Action.
	ArpgInputComponent->BindAction(moveAction, ETriggerEvent::Triggered, this, &AarpgPlayerController::Move);

	ArpgInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}

void AarpgPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();

	const FRotator Rotation = GetControlRotation();
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
