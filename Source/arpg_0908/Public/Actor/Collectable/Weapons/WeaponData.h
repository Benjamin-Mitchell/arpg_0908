// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "ArpgWeaponActor.h"
#include "Abilities/GameplayAbility.h"
#include "Engine/DataAsset.h"
#include "WeaponData.generated.h"


USTRUCT(BlueprintType)
struct FWeaponInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category="Test")
	TSubclassOf<AArpgWeaponActor> WeaponReference;
};
/**
 * 
 */
UCLASS()
class ARPG_0908_API UWeaponData : public UDataAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditDefaultsOnly, Category="Heads")
	TArray<FWeaponInfo> Weapons;

	FWeaponInfo GetWeaponInfo(int Index);

	int GetWeaponIndex(AArpgWeaponActor* WeaponActor);
};
