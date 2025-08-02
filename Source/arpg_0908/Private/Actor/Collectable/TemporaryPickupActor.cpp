// Copyright Tinzie Games


#include "Actor/Collectable/TemporaryPickupActor.h"

// Sets default values
ATemporaryPickupActor::ATemporaryPickupActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATemporaryPickupActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATemporaryPickupActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

