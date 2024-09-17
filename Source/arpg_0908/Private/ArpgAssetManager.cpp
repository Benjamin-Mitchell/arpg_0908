// Copyright Tinzie Games


#include "ArpgAssetManager.h"

#include "AbilitySystemGlobals.h"
#include "ArpgGameplayTags.h"

UArpgAssetManager& UArpgAssetManager::Get()
{
	check(GEngine);
	UArpgAssetManager* ArpgAssetManager = Cast<UArpgAssetManager>(GEngine->AssetManager);
	return *ArpgAssetManager;
	
}

void UArpgAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	FArpgGameplayTags::InitializeNativeGameplayTags();

	//This is required to use Target Data!
	UAbilitySystemGlobals::Get().InitGlobalData();
}
