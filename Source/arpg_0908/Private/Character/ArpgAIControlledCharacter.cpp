// Copyright Tinzie Games


#include "Character/ArpgAIControlledCharacter.h"

#include "AbilitySystem/arpgAbilitySystemComponent.h"
#include "AbilitySystem/arpgAttributeSet.h"
#include "Components/WidgetComponent.h"
#include "UI/Widget/arpgUserWidget.h"
#include "ArpgGameplayTags.h"
#include "AbilitySystem/ArpgAbilitySystemLibrary.h"
#include "AI/ArpgAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AarpgAIControlledCharacter::AarpgAIControlledCharacter()
{
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);


	

	AbilitySystemComponent = CreateDefaultSubobject<UarpgAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bUseControllerDesiredRotation = true;

	
	AttributeSet = CreateDefaultSubobject<UarpgAttributeSet>("AttributeSet");

	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBar->SetupAttachment(GetRootComponent());

	DurationBar = CreateDefaultSubobject<UWidgetComponent>("DurationBar");
	DurationBar->SetupAttachment(GetRootComponent());
	
	TargettedIcon = CreateDefaultSubobject<UWidgetComponent>("TargettedIcon");
    TargettedIcon ->SetupAttachment(GetMesh(), FName("UpperChestSocket"));
}

void AarpgAIControlledCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if(!HasAuthority()) return;

	//enemy uses flat values for highlight colours
	GetMesh()->SetCustomDepthStencilValue(MeshBaseHighlightVal);
	Weapon->SetCustomDepthStencilValue(WeaponBaseHighlightVal);
	BaseHeadMesh->SetCustomDepthStencilValue(HeadBaseHighlightVal);

	ArpgAIController = Cast<AArpgAIController>(NewController);
	ArpgAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	
	ArpgAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), false);

	if (InitialTempDurationLifeSpan > 0.0f)
	{
		ArpgAIController->GetBlackboardComponent()->SetValueAsBool(FName("TempDurationOver"), false);

		//TODO: handle being able to 'reset' the lifespan or a temporary character, to keep it alive for a while (Set function for replicated variable?)
		GetWorld()->GetTimerManager().SetTimer(TempDurationTimerHandle, this, &AarpgAIControlledCharacter::TriggerTempDurationOver, InitialTempDurationLifeSpan, false);		
	}
}

void AarpgAIControlledCharacter::HighlightActor()
{
	GetMesh()->SetCustomDepthStencilValue(250);
	Weapon->SetCustomDepthStencilValue(250);
}
void AarpgAIControlledCharacter::UnHighlightActor()
{
	GetMesh()->SetCustomDepthStencilValue(MeshBaseHighlightVal);
	Weapon->SetCustomDepthStencilValue(WeaponBaseHighlightVal);
}

void AarpgAIControlledCharacter::Interact(AarpgPlayerController* InteractingPlayer)
{
}

int32 AarpgAIControlledCharacter::GetPlayerLevel()
{
	return Level;
}

void AarpgAIControlledCharacter::Die(const FVector& DeathImpulse)
{
	SetLifeSpan(PostDeathLifeSpan);

	if(ArpgAIController)
		ArpgAIController->GetBlackboardComponent()->SetValueAsBool(FName("Dead"), true);
	
	Super::Die(DeathImpulse);
}

void AarpgAIControlledCharacter::SetCombatTarget_Implementation(AActor* InCombatTarget)
{
	CombatTarget = InCombatTarget;
}

AActor* AarpgAIControlledCharacter::GetCombatTarget_Implementation() const
{
	return CombatTarget;
}


void AarpgAIControlledCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	InitAbilityActorInfo();

	if(HasAuthority())
	{
		UArpgAbilitySystemLibrary::GiveEnemyAbilities(this, AbilitySystemComponent, StartupAbilities,  EnemyUtilityAbilities);
	}

	if(UarpgUserWidget* ArpgUserWidget = Cast<UarpgUserWidget>(HealthBar->GetUserWidgetObject()))
	{
		ArpgUserWidget->SetWidgetController(this);
	}

	if(UarpgUserWidget* ArpgUserWidget = Cast<UarpgUserWidget>(TargettedIcon->GetUserWidgetObject()))
	{
		ArpgUserWidget->SetWidgetController(this);
	}
	
	if(UarpgUserWidget* ArpgUserWidget = Cast<UarpgUserWidget>(DurationBar->GetUserWidgetObject()))
	{
		ArpgUserWidget->SetWidgetController(this);
	}

	if (InitialTempDurationLifeSpan > 0.0f)
	{
		//Let the duration bar know temp duration has begun.
		//Note: Actual timer is handled in PossessedBy, by the server.
		OnTempDurationBegan.Broadcast(InitialTempDurationLifeSpan);
	}

	if(const UarpgAttributeSet* ArpgAS = CastChecked<UarpgAttributeSet>(AttributeSet))
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(ArpgAS->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			});

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(ArpgAS->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			});

		
		AbilitySystemComponent->RegisterGameplayTagEvent(FArpgGameplayTags::Get().Effects_HitReact, EGameplayTagEventType::NewOrRemoved).AddUObject(
			this,
			&AarpgAIControlledCharacter::HitReactTagChanged
		);

		OnHealthChanged.Broadcast(ArpgAS->GetHealth());
		OnMaxHealthChanged.Broadcast(ArpgAS->GetMaxHealth());
	}
}

void AarpgAIControlledCharacter::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bHitReacting = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;

	if(ArpgAIController && ArpgAIController->GetBlackboardComponent())
		ArpgAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), bHitReacting);
}

void AarpgAIControlledCharacter::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	AbilitySystemComponent->AbilityActorInfoSet();
	AbilitySystemComponent->RegisterGameplayTagEvent(FArpgGameplayTags::Get().Debuff_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AarpgAIControlledCharacter::StunTagChanged);
	AbilitySystemComponent->RegisterGameplayTagEvent(FArpgGameplayTags::Get().Immunity_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AarpgAIControlledCharacter::StunImmunityTagChanged);
	
	if(HasAuthority())
	{
		InitializeDefaultAttributes();
	}

	GetOnASCRegisteredDelegate().Broadcast(AbilitySystemComponent);
}

void AarpgAIControlledCharacter::InitializeDefaultAttributes() const
{
	//Just initialize them with a specific list per enemy, this makes more sense anyway.
	Super::InitializeDefaultAttributes();
}

void AarpgAIControlledCharacter::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{	
	Super::StunTagChanged(CallbackTag, NewCount);
	
	if(ArpgAIController && ArpgAIController->GetBlackboardComponent())
		ArpgAIController->GetBlackboardComponent()->SetValueAsBool(FName("Stunned"), bIsStunned);
}

void AarpgAIControlledCharacter::StartBlackboard()
{
	ArpgAIController->RunBehaviorTree(BehaviorTree);
}

void AarpgAIControlledCharacter::GiveAndActivateSpawnAbility()
{
	FGameplayAbilitySpec SpawnAbilitySpec = FGameplayAbilitySpec(SpawnAbility, 1);

	AbilitySystemComponent->GiveAbilityAndActivateOnce(SpawnAbilitySpec);
}

void AarpgAIControlledCharacter::GiveDespawnAbility()
{
	if (IsValid(DeSpawnAbility))
	{
		FGameplayAbilitySpec DeSpawnAbilitySpec = FGameplayAbilitySpec(DeSpawnAbility, 1);
		AbilitySystemComponent->GiveAbility((DeSpawnAbilitySpec));
	}	
}

void AarpgAIControlledCharacter::SetBlackboardInAir(bool bInAir)
{
	ArpgAIController->GetBlackboardComponent()->SetValueAsBool(FName("InAir"), bInAir);
}

void AarpgAIControlledCharacter::SetBlackboardCustomBool(FName BoolName, bool Val)
{
	if (HasAuthority())
	{
		ArpgAIController->GetBlackboardComponent()->SetValueAsBool(BoolName, Val);
	}
}

void AarpgAIControlledCharacter::SetBlackboardCustomObject(FName KeyName, UObject* Obj)
{
	if (HasAuthority())
	{
		ArpgAIController->GetBlackboardComponent()->SetValueAsObject(KeyName, Obj);
	}
}

void AarpgAIControlledCharacter::TriggerTempDurationOver()
{
	ArpgAIController->GetBlackboardComponent()->SetValueAsBool(FName("TempDurationOver"), true);
}
