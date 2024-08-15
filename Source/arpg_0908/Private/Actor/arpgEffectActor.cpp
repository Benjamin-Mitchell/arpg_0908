// Copyright Tinzie Games


#include "Actor/arpgEffectActor.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/arpgAttributeSet.h"
#include "Components/SphereComponent.h"
#include "PhysicsEngine/ShapeElem.h"

AarpgEffectActor::AarpgEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);
	
	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	Sphere->SetupAttachment(GetRootComponent());
	
}

void AarpgEffectActor::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//TODO: change this to apply a Gameplay Effect. For now, using const_cast as a Hack!!
	//Other actor has an ASC Interface.
	if(IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(OtherActor))
	{
		//Get Attribute Set searches for, and returns if valid, an attribute set object of type UarpgAttributeSet
		const UarpgAttributeSet* aarpgAttributeSet = Cast<UarpgAttributeSet>(ASCInterface->GetAbilitySystemComponent()->GetAttributeSet(UarpgAttributeSet::StaticClass()));

		UarpgAttributeSet* MutableArpgAttributeSet = const_cast<UarpgAttributeSet*>(aarpgAttributeSet);
		MutableArpgAttributeSet->SetHealth(aarpgAttributeSet->GetHealth() + 25.0f);

		Destroy();
	}
}

void AarpgEffectActor::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}

void AarpgEffectActor::BeginPlay()
{
	Super::BeginPlay();

	//Adding a function for on collision overlap.
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AarpgEffectActor::OnOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AarpgEffectActor::EndOverlap);
}

