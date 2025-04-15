// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"

#define CUSTOM_DEPTH_RED 250
#define ECC_Projectile ECollisionChannel::ECC_GameTraceChannel1
#define ECC_Target ECollisionChannel::ECC_GameTraceChannel2


//Debug print
//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("boobs"));
//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Health NewValue: %f"), NewValue));

//DrawDebugSphere(World, Location , 20, 30, FColor::Blue, false, 1.0f);

class FArpgModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
