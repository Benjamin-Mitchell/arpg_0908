// Copyright Tinzie Games


#include "Player/arpgPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"


AarpgPlayerController::AarpgPlayerController()
{
	bReplicates = true;
}

void AarpgPlayerController::BeginPlay()
{
	Super::BeginPlay();

	//assert input mapping context is already set
	check(arpgIMContext);

	//Subsystems are singletons. They're the system for the local controlled player.
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	check(Subsystem);

	Subsystem->AddMappingContext(arpgIMContext, 0);

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

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	//Execute the Move Function whenever a Moove button is pressed in the Input Action.
	EnhancedInputComponent->BindAction(moveAction, ETriggerEvent::Triggered, this, &AarpgPlayerController::Move);
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
