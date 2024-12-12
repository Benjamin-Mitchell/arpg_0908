// Copyright Tinzie Games


#include "Character/arpgCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "ArpgGameplayTags.h"
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

AActor* AarpgCharacterBase::GetAvatar_Implementation()
{
	return this;
}

UAnimMontage* AarpgCharacterBase::GetHitReactMontage_Implementation()
{
	return HitReactMontage;
}

void AarpgCharacterBase::Die()
{
	Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	MulticastHandleDeath();
}

TArray<FTaggedMontage> AarpgCharacterBase::GetAttackMontages_Implementation()
{
	return AttackMontages;
}

void AarpgCharacterBase::MulticastHandleDeath_Implementation()
{
	Weapon->SetSimulatePhysics(true);
	Weapon->SetEnableGravity(true);
	Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Dissolve();
	bDead = true;
}

void AarpgCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

FVector AarpgCharacterBase::GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag)
{
	const FArpgGameplayTags& GameplayTags = FArpgGameplayTags::Get();
	if(MontageTag.MatchesTagExact(GameplayTags.Montage_Attack_Weapon) && IsValid(Weapon))
	{
		return Weapon->GetSocketLocation(WeaponTipSocketName);
	}
	if(MontageTag.MatchesTagExact(GameplayTags.Montage_Attack_LeftHand))
	{
		return GetMesh()->GetSocketLocation(LeftHandSocketName);
	}
	if(MontageTag.MatchesTagExact(GameplayTags.Montage_Attack_RightHand))
	{
		return GetMesh()->GetSocketLocation(RightHandSocketName);
	}

	return FVector();
}

bool AarpgCharacterBase::IsDead_Implementation() const
{
	return bDead;
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

void AarpgCharacterBase::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>> &Abilities)
{
	if(!HasAuthority()) return;

	UarpgAbilitySystemComponent* ArpgASC = CastChecked<UarpgAbilitySystemComponent>(AbilitySystemComponent);

	ArpgASC->AddCharacterAbilities(Abilities);
}

void AarpgCharacterBase::RemoveCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>> &Abilities)
{
	if(!HasAuthority()) return;

	UarpgAbilitySystemComponent* ArpgASC = CastChecked<UarpgAbilitySystemComponent>(AbilitySystemComponent);

	ArpgASC->RemoveCharacterAbilities(Abilities);
}

void AarpgCharacterBase::SetIsTraversing(const bool& bInIsTraversing, const FTransform InTarget)
{
	TraversalTarget = InTarget;
	bIsTraversing = bInIsTraversing;
}

void AarpgCharacterBase::Dissolve()
{
	if(IsValid(DissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DynamicMatInst);

		StartDissolveTimeline(DynamicMatInst);
	}
	
	if(IsValid(WeaponDissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(WeaponDissolveMaterialInstance, this);
		Weapon->SetMaterial(0, DynamicMatInst);

		StartWeaponDissolveTimeline(DynamicMatInst);
	}
}

