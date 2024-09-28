// Copyright Tinzie Games


#include "Actor/ProjectileTypes/HookProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "ArpgGameplayTags.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/arpgCharacter.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

void AHookProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!bHit)
	{
		if (LoopingSoundComponent) LoopingSoundComponent->Stop();
		if (ImpactSound) UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), GetActorRotation());
		if (ImpactEffect) UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation(), GetActorRotation());	
	}

	if(HasAuthority())
	{
		if(UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			if(AarpgCharacter* Character = Cast<AarpgCharacter>(OtherActor))
			{
				Character->SetIsTraversing(true, HookStartTransform);
				
				//Activate any abilities with the "effects_HookPull" tag on the target
				FGameplayTagContainer TagContainer;
				TagContainer.AddTag(FArpgGameplayTags::Get().Effects_HookPull);
				TargetASC->TryActivateAbilitiesByTag(TagContainer);

				
			}
		}
		
		Destroy();
	}
	else
	{
		bHit = true;
	}
}
