// Copyright Tinzie Games


#include "Actor/ArpgAbilityGranterActor.h"

#include "Character/arpgCharacterBase.h"

// Sets default values
AArpgAbilityGranterActor::AArpgAbilityGranterActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or wheActivateAbilitiesImmediatelyn spawned
void AArpgAbilityGranterActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AArpgAbilityGranterActor::GrantAbilityToTarget(AActor* TargetActor)
{	
	// Check if the overlapping actor is a character
	AarpgCharacterBase* Character = Cast<AarpgCharacterBase>(TargetActor);

	if (Character)
	{
		Character->AddCharacterAbilities(AbilitiesToGrant, ActivateAbilitiesImmediately);

		//Play particle effects here
		Destroy();
	}
}

void AArpgAbilityGranterActor::OnOverlap(AActor* TargetActor)
{
	//ignore for enemies if player only or vice-versa
	for(FName Tag : TargetTagsToIgnore)
	{
		if(TargetActor->ActorHasTag(Tag))
			return;
	}
	
	GrantAbilityToTarget(TargetActor);
}

void AArpgAbilityGranterActor::OnEndOverlap(AActor* TargetActor)
{
	//Shouldn't happen for now
	//GrantAbilityToTarget(TargetActor);
	
}

// Called every frame
void AArpgAbilityGranterActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

