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
	
	TargettedIcon = CreateDefaultSubobject<UWidgetComponent>("TargettedIcon");
    TargettedIcon ->SetupAttachment(GetMesh(), FName("UpperChestSocket"));
}

void AarpgAIControlledCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if(!HasAuthority()) return;

	//enemy uses flat values for highlight colours
	GetMesh()->SetCustomDepthStencilValue(BaseHighlightVal);
	Weapon->SetCustomDepthStencilValue(WeaponBaseHighlightVal);
	BaseHeadMesh->SetCustomDepthStencilValue(HeadBaseHighlightVal);

	ArpgAIController = Cast<AArpgAIController>(NewController);
	ArpgAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	
	ArpgAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), false);

	ArpgAIController->GetBlackboardComponent()->SetValueAsBool(FName("RangedAttacker"), CharacterClass != ECharacterClass::Warrior);
}

void AarpgAIControlledCharacter::HighlightActor()
{
	GetMesh()->SetCustomDepthStencilValue(250);
	Weapon->SetCustomDepthStencilValue(250);
}
void AarpgAIControlledCharacter::UnHighlightActor()
{
	GetMesh()->SetCustomDepthStencilValue(BaseHighlightVal);
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
	SetLifeSpan(LifeSpan);

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
		//To be honest, probably remove this whole character class system nonsense later, its just used for the course mobs.
		if (CharacterClass != ECharacterClass::NONE)
		{
			UArpgAbilitySystemLibrary::GiveStartupAbilities(this, AbilitySystemComponent, CharacterClass);
		}
		else
		{
			UArpgAbilitySystemLibrary::GiveEnemyAbilities(this, AbilitySystemComponent, StartupAbilities,  EnemyUtilityAbilities);
		}
	}

	if(UarpgUserWidget* ArpgUserWidget = Cast<UarpgUserWidget>(HealthBar->GetUserWidgetObject()))
	{
		ArpgUserWidget->SetWidgetController(this);
	}

	if(UarpgUserWidget* ArpgUserWidget = Cast<UarpgUserWidget>(TargettedIcon->GetUserWidgetObject()))
	{
		ArpgUserWidget->SetWidgetController(this);
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

	Cast<UarpgAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
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
	//To be honest, probably remove this whole character class system nonsense later, its just used for the course mobs.
	if (CharacterClass != ECharacterClass::NONE)
	{
		UArpgAbilitySystemLibrary::InitializeDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);
	}
	else
	{
		//Just initialize them with a specific list per enemy, this makes more sense anyway.
		Super::InitializeDefaultAttributes();
	}
	
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
