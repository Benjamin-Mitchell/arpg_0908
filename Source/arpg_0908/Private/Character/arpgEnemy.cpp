// Copyright Tinzie Games


#include "Character/arpgEnemy.h"

#include "AbilitySystem/arpgAbilitySystemComponent.h"
#include "AbilitySystem/arpgAttributeSet.h"
#include "Components/WidgetComponent.h"
#include "UI/Widget/arpgUserWidget.h"

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

//TODO: we don't need to SetRenderCustomDepth true and false every time..
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

	InitAbilityActorInfo();

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

		OnHealthChanged.Broadcast(ArpgAS->GetHealth());
		OnMaxHealthChanged.Broadcast(ArpgAS->GetMaxHealth());
	}
}

void AarpgEnemy::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	Cast<UarpgAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();

	InitializeDefaultAttributes();
}
