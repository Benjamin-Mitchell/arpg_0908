// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "arpgEffectActor.generated.h"

class UGameplayEffect;
class USphereComponent;

UCLASS()
class ARPG_0908_API AarpgEffectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AarpgEffectActor();
	
protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GamePlayEffectClass);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	TSubclassOf<UGameplayEffect> InstantGameplayEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	TSubclassOf<UGameplayEffect> DurationGameplayEffectClass;

};
