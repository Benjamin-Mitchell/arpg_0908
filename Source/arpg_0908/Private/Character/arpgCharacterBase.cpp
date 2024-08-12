// Copyright Tinzie Games


#include "Character/arpgCharacterBase.h"

AarpgCharacterBase::AarpgCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;
	
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

UAbilitySystemComponent* AarpgCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AarpgCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

