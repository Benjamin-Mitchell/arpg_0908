// Copyright Tinzie Games


#include "Character/arpgCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "ArpgGameplayTags.h"
#include "AbilitySystem/arpgAbilitySystemComponent.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "arpg_0908/arpg_0908.h"
#include "Components/CapsuleComponent.h"

AarpgCharacterBase::AarpgCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	BurnNiagaraComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>("BurnDebuffComponent");;
	BurnNiagaraComponent->SetupAttachment(GetRootComponent());
	BurnNiagaraComponent->DebuffTag = FArpgGameplayTags::Get().Debuff_Burn;
	
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Weapon->SetRenderCustomDepth(true);
	
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetMesh()->SetRenderCustomDepth(true);

	
	
	BaseHeadMesh = CreateDefaultSubobject<USkeletalMeshComponent>("HeadMesh");
	BaseHeadMesh->SetupAttachment(GetMesh(), FName("HeadSocket"));
	BaseHeadMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BaseHeadMesh->SetRenderCustomDepth(true);

	
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


void AarpgCharacterBase::Die(const FVector& DeathImpulse)
{
	Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	MulticastHandleDeath(DeathImpulse);
}

TArray<FTaggedMontage> AarpgCharacterBase::GetAttackMontages_Implementation()
{
	return AttackMontages;
}

FOnASCRegistered AarpgCharacterBase::GetOnASCRegisteredDelegate()
{
	return OnASCRegistered;
}

FOnDeath AarpgCharacterBase::GetOnDeathDelegate()
{
	return OnDeath;
}

USkeletalMeshComponent* AarpgCharacterBase::GetWeapon_Implementation()
{
	return Weapon;
}

void AarpgCharacterBase::MulticastHandleDeath_Implementation(const FVector& DeathImpulse)
{
	Weapon->SetSimulatePhysics(true);
	Weapon->SetEnableGravity(true);
	Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	Weapon->AddImpulse(DeathImpulse * 0.1f, NAME_None, true);

	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetMesh()->AddImpulse(DeathImpulse, NAME_None, true);
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Dissolve();
	bDead = true;
	OnDeath.Broadcast(this);
}

void AarpgCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

FVector AarpgCharacterBase::GetCombatSocketLocation_Implementation(const FGameplayTag MontageTag)
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

	if(MontageTag.MatchesTagExact(GameplayTags.Montage_Attack_DamageTraceBegin))
	{
		return GetMesh()->GetSocketLocation(TraceBeginSocketName);
	}

	if(MontageTag.MatchesTagExact(GameplayTags.Montage_Attack_DamageTraceEnd))
	{
		return GetMesh()->GetSocketLocation(TraceEndSocketName);
	}

	if(MontageTag.MatchesTagExact(GameplayTags.Montage_Attack_CustomOrDebug))
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("got the correct tag"));
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Health NewValue: %s"), *CustomOrDebugSocketName.ToString()));
		return GetMesh()->GetSocketLocation(CustomOrDebugSocketName);
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

