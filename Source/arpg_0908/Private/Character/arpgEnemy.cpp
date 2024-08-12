// Copyright Tinzie Games


#include "Character/arpgEnemy.h"

#include "AbilitySystem/arpgAbilitySystemComponent.h"
#include "AbilitySystem/arpgAttributeSet.h"
#include "arpg_0908/arpg_0908.h"

AarpgEnemy::AarpgEnemy()
{
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetRenderCustomDepth(true);
	Weapon->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(0);
	Weapon->SetCustomDepthStencilValue(0);

	AbilitySystemComponent = CreateDefaultSubobject<UarpgAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);

	AttributeSet = CreateDefaultSubobject<UarpgAttributeSet>("AttributeSet");
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