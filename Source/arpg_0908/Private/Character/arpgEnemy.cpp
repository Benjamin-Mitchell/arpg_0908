// Copyright Tinzie Games


#include "Character/arpgEnemy.h"

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

AarpgEnemy::AarpgEnemy()
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

void AarpgEnemy::PossessedBy(AController* NewController)
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

void AarpgEnemy::HighlightActor()
{
	GetMesh()->SetCustomDepthStencilValue(250);
	Weapon->SetCustomDepthStencilValue(250);
}
void AarpgEnemy::UnHighlightActor()
{
	GetMesh()->SetCustomDepthStencilValue(BaseHighlightVal);
	Weapon->SetCustomDepthStencilValue(WeaponBaseHighlightVal);
}

void AarpgEnemy::Interact(AarpgPlayerController* InteractingPlayer)
{
}

int32 AarpgEnemy::GetPlayerLevel()
{
	return Level;
}

void AarpgEnemy::Die(const FVector& DeathImpulse)
{
	SetLifeSpan(LifeSpan);

	if(ArpgAIController)
		ArpgAIController->GetBlackboardComponent()->SetValueAsBool(FName("Dead"), true);
	
	Super::Die(DeathImpulse);
}

void AarpgEnemy::SetCombatTarget_Implementation(AActor* InCombatTarget)
{
	CombatTarget = InCombatTarget;
}

AActor* AarpgEnemy::GetCombatTarget_Implementation() const
{
	return CombatTarget;
}


void AarpgEnemy::BeginPlay()
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
			&AarpgEnemy::HitReactTagChanged
		);

		OnHealthChanged.Broadcast(ArpgAS->GetHealth());
		OnMaxHealthChanged.Broadcast(ArpgAS->GetMaxHealth());
	}
}

void AarpgEnemy::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bHitReacting = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;

	if(ArpgAIController && ArpgAIController->GetBlackboardComponent())
		ArpgAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), bHitReacting);
}

void AarpgEnemy::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	Cast<UarpgAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
	AbilitySystemComponent->RegisterGameplayTagEvent(FArpgGameplayTags::Get().Debuff_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AarpgEnemy::StunTagChanged);
	AbilitySystemComponent->RegisterGameplayTagEvent(FArpgGameplayTags::Get().Immunity_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AarpgEnemy::StunImmunityTagChanged);
	
	if(HasAuthority())
	{
		InitializeDefaultAttributes();
	}

	GetOnASCRegisteredDelegate().Broadcast(AbilitySystemComponent);
}

void AarpgEnemy::InitializeDefaultAttributes() const
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

void AarpgEnemy::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{	
	Super::StunTagChanged(CallbackTag, NewCount);
	
	if(ArpgAIController && ArpgAIController->GetBlackboardComponent())
		ArpgAIController->GetBlackboardComponent()->SetValueAsBool(FName("Stunned"), bIsStunned);
}

void AarpgEnemy::StartBlackboard()
{
	ArpgAIController->RunBehaviorTree(BehaviorTree);
}

void AarpgEnemy::SetBlackboardInAir(bool bInAir)
{
	ArpgAIController->GetBlackboardComponent()->SetValueAsBool(FName("InAir"), bInAir);
}

void AarpgEnemy::SetBlackboardCustomBool(FName BoolName, bool Val)
{
	if (HasAuthority())
	{
		ArpgAIController->GetBlackboardComponent()->SetValueAsBool(BoolName, Val);
	}
}

void AarpgEnemy::SetBlackboardCustomObject(FName KeyName, UObject* Obj)
{
	if (HasAuthority())
	{
		ArpgAIController->GetBlackboardComponent()->SetValueAsObject(KeyName, Obj);
	}
}
