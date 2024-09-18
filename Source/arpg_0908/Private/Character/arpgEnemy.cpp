// Copyright Tinzie Games


#include "Character/arpgEnemy.h"

#include "AbilitySystem/arpgAbilitySystemComponent.h"
#include "AbilitySystem/arpgAttributeSet.h"
#include "Components/WidgetComponent.h"
#include "UI/Widget/arpgUserWidget.h"
#include "ArpgGameplayTags.h"
#include "AbilitySystem/ArpgAbilitySystemLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

AarpgEnemy::AarpgEnemy()
{
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetRenderCustomDepth(true);
	Weapon->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(0);
	Weapon->SetCustomDepthStencilValue(0);

	AbilitySystemComponent = CreateDefaultSubobject<UarpgAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UarpgAttributeSet>("AttributeSet");

	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBar->SetupAttachment(GetRootComponent());
}

void AarpgEnemy::HighlightActor()
{
	GetMesh()->SetCustomDepthStencilValue(250);
	Weapon->SetCustomDepthStencilValue(250);
}
void AarpgEnemy::UnHighlightActor()
{
	GetMesh()->SetCustomDepthStencilValue(0);
	Weapon->SetCustomDepthStencilValue(0);
}

int32 AarpgEnemy::GetPlayerLevel()
{
	return Level;
}



void AarpgEnemy::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	InitAbilityActorInfo();
	UArpgAbilitySystemLibrary::GiveStartupAbilities(this, AbilitySystemComponent);

	if(UarpgUserWidget* ArpgUserWidget = Cast<UarpgUserWidget>(HealthBar->GetUserWidgetObject()))
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
}

void AarpgEnemy::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	Cast<UarpgAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();

	InitializeDefaultAttributes();
}

void AarpgEnemy::InitializeDefaultAttributes() const
{
	UArpgAbilitySystemLibrary::InitializeDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);
}
