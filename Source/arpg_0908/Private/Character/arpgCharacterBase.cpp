// Copyright Tinzie Games


#include "Character/arpgCharacterBase.h"

AarpgCharacterBase::AarpgCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;
	
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AarpgCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

