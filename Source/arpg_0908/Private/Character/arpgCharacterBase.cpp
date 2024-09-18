// Copyright Tinzie Games


#include "Character/arpgCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/arpgAbilitySystemComponent.h"
#include "arpg_0908/arpg_0908.h"
#include "Components/CapsuleComponent.h"

AarpgCharacterBase::AarpgCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;
	
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);
}

UAbilitySystemComponent* AarpgCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAnimMontage* AarpgCharacterBase::GetHitReactMontage_Implementation()
{
	return HitReactMontage;
}

void AarpgCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

FVector AarpgCharacterBase::GetCombatSocketLocation()
{
	check(Weapon);
	return Weapon->GetSocketLocation(WeaponTipSocketName);
}

void AarpgCharacterBase::InitAbilityActorInfo()
{
}

void AarpgCharacterBase::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const
{
	check(IsValid(GetAbilitySystemComponent()));
	check(GameplayEffectClass);
	FGameplayEffectContextHandle EffectContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass, Level, EffectContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());	
}

void AarpgCharacterBase::InitializeDefaultAttributes() const
{
	ApplyEffectToSelf(DefaultPrimaryAttributes, 1.0f);

	//Derives attributes must be set after primary, as they depend on primary
	ApplyEffectToSelf(DefaultDerivedAttributes, 1.0f);

	//Vital attributes must be set last as they depend on other attributes
	ApplyEffectToSelf(DefaultVitalAttributes, 1.0f);
	
}

void AarpgCharacterBase::AddCharacterAbilities()
{
	if(!HasAuthority()) return;

	UarpgAbilitySystemComponent* ArpgASC = CastChecked<UarpgAbilitySystemComponent>(AbilitySystemComponent);

	ArpgASC->AddCharacterAbilities(StartupAbilities);
}

