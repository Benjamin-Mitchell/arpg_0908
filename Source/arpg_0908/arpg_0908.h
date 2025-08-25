// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"

#define CUSTOM_DEPTH_RED 250
#define ECC_Projectile ECollisionChannel::ECC_GameTraceChannel1
#define ECC_Target ECollisionChannel::ECC_GameTraceChannel2
#define ECC_StaticGeometryTrace ECollisionChannel::ECC_GameTraceChannel3


//Debug print
//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("boobs"));
//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Health NewValue: %f"), NewValue));

//const UWorld* World = GEngine->GetWorldFromContextObject(AvatarActor, EGetWorldErrorMode::LogAndReturnNull);
//DrawDebugSphere(World, Location , 20, 30, FColor::Blue, false, 1.0f);

class FArpgModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
