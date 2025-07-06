// Copyright Tinzie Games


#include "Character/arpgCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "ArpgGameplayTags.h"
#include "AbilitySystem/arpgAbilitySystemComponent.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "arpg_0908/arpg_0908.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "UI/Widget/ArpgTemporaryTextComponent.h"

AarpgCharacterBase::AarpgCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	BurnNiagaraComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>("BurnDebuffComponent");;
	BurnNiagaraComponent->SetupAttachment(GetRootComponent());
	BurnNiagaraComponent->DebuffTag = FArpgGameplayTags::Get().Debuff_Burn;

	StunNiagaraComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>("StunDebuffComponent");;
	StunNiagaraComponent->SetupAttachment(GetRootComponent());
	StunNiagaraComponent->DebuffTag = FArpgGameplayTags::Get().Debuff_Stun;

	GrabLocationComponent = CreateDefaultSubobject<USceneComponent>("GrabLocationComponent");
	GrabLocationComponent->SetupAttachment(GetRootComponent());
	
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

void AarpgCharacterBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AarpgCharacterBase, bIsStunned);
	DOREPLIFETIME(AarpgCharacterBase, bIsBurning);
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

FOnASCRegistered& AarpgCharacterBase::GetOnASCRegisteredDelegate()
{
	return OnASCRegistered;
}

FOnDeathSignature& AarpgCharacterBase::GetOnDeathDelegate()
{
	return OnDeath;
}

USkeletalMeshComponent* AarpgCharacterBase::GetWeapon_Implementation()
{
	return Weapon;
}

bool AarpgCharacterBase::IsInPlay_Implementation() const
{
	return bInPlay;
}

void AarpgCharacterBase::SetPassiveAbilityTarget_Implementation(AActor* InCombatTarget)
{
	PassiveAbilityTarget = InCombatTarget;
}

AActor* AarpgCharacterBase::GetPassiveAbilityTarget_Implementation() const
{
	return PassiveAbilityTarget;
}

void AarpgCharacterBase::ServerSetClientBeginSnapToTargetSocket(AarpgCharacterBase* Target, const FName SocketName, const float SnappingDuration)
{
	MulticastBeginSnapToTargetSocket(Target, SocketName, SnappingDuration);
}

void AarpgCharacterBase::ServerSetClientEndSnapToTargetSocket()
{
	MulticastEndSnapToTargetSocket();
}


void AarpgCharacterBase::MulticastBeginSnapToTargetSocket_Implementation(AarpgCharacterBase* SnapToTargetActor, const FName SocketName, const float SnappingDuration)
{
	GetWorld()->GetTimerManager().ClearTimer(SnapTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(OptionalSnapDurationHandle);

	GetCapsuleComponent()->SetWorldRotation(FRotator::ZeroRotator);

	FVector GrabOffset = DefaultGrabOffsetToMesh;
	//start a timer
	GetWorld()->GetTimerManager().SetTimer(
	SnapTimerHandle,
	[this, SnapToTargetActor, SocketName, GrabOffset]() 
		{
			//New
			//Remove relative offset of our mesh.
			GetMesh()->SetRelativeLocation(FVector::Zero());

			FTransform SocketTransform = SnapToTargetActor->GetMesh()->GetSocketTransform(SocketName);
			//Move entire Actor to SocketName's Location.
			SetActorLocation(SocketTransform.GetLocation());
			
			//Rotate Our Mesh to SocketName's rotation.
			FRotator SocketRotation = SocketTransform.GetRotation().Rotator();
			GetMesh()->SetWorldRotation(SocketRotation);
			
			FVector RotatedGrabOffset = SocketRotation.RotateVector(GrabOffset);
			GetMesh()->AddRelativeLocation(RotatedGrabOffset);
			
		},
	0.01f,
	true // Do loop
	);


	//Snapping Duration is optional. if the duration is negative, abilities should end the snapping themselves.
	if (SnappingDuration > 0.0f)
	{
		//start a timer
		GetWorld()->GetTimerManager().SetTimer(
		OptionalSnapDurationHandle,
		[this]() 
			{
				GetWorld()->GetTimerManager().ClearTimer(SnapTimerHandle);
				GetWorld()->GetTimerManager().ClearTimer(OptionalSnapDurationHandle);
			},
		SnappingDuration,
		false // Don't loop
		);
	}
}

void AarpgCharacterBase::MulticastEndSnapToTargetSocket_Implementation()
{
	//clear timer
	GetWorld()->GetTimerManager().ClearTimer(SnapTimerHandle);
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

	BurnNiagaraComponent->Deactivate();
	StunNiagaraComponent->Deactivate();
	
	Dissolve();
	bDead = true;
	OnDeath.Broadcast(this);
}

void AarpgCharacterBase::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bIsStunned = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bIsStunned ? 0.f : BaseWalkSpeed;
}

void AarpgCharacterBase::Onrep_Stunned()
{
	
}

void AarpgCharacterBase::Onrep_Burned()
{
}

void AarpgCharacterBase::SetCapsulePawnCollisionEnabled(const bool Enabled)
{
	if (Enabled)
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	else
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AarpgCharacterBase::SetInPlay(const bool InIsInPlay)
{
	bInPlay = InIsInPlay;
}

FVector AarpgCharacterBase::GetGrabRelativeLocation()
{
	return GrabLocationComponent->GetRelativeLocation();
}

void AarpgCharacterBase::ServerResetMeshRelativeTransformToDefault(float ResetDuration)
{
	//Send dispatch to all the clients to reset the transformation
	MulticastResetMeshRelativeTransformToDefault(ResetDuration);

	//Then handle it locally
	ResetMeshRelativeTransformToDefault(ResetDuration);
}

void AarpgCharacterBase::ResetMeshRelativeTransformToDefault(float ResetDuration)
{
	if (ResetDuration < 0.0f)
		GetMesh()->SetRelativeTransform(DefaultMeshRelativeTransform);
	else
	{

		FTransform MeshStartRelativeTransform = GetMesh()->GetRelativeTransform();
		RelativeTransformAlpha = 0.0f;
		float TimeStep = 0.05f;
		
		//start a timer
		GetWorld()->GetTimerManager().SetTimer(
		MeshRelativeTransformTimerHandle,
		[this, MeshStartRelativeTransform, TimeStep, ResetDuration]() 
			{
				RelativeTransformAlpha += TimeStep;
				float ActualAlpha = RelativeTransformAlpha / ResetDuration;
				FTransform Res;
				Res.Blend(MeshStartRelativeTransform, DefaultMeshRelativeTransform, RelativeTransformAlpha);

				GetMesh()->SetRelativeTransform(Res);

				if (ActualAlpha >= 1.0f)
					GetWorld()->GetTimerManager().ClearTimer(MeshRelativeTransformTimerHandle);
			},
		TimeStep,
		true // Do loop
		);
	}

	
}

void AarpgCharacterBase::MulticastResetMeshRelativeTransformToDefault_Implementation(float ResetDuration)
{
	ResetMeshRelativeTransformToDefault(ResetDuration);
}

void AarpgCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->AddLooseGameplayTags(ObjectTypeTags);
	}
	else
	{
		GetOnASCRegisteredDelegate().AddWeakLambda(this, [this](UAbilitySystemComponent* InASC)
			{
				AbilitySystemComponent->AddLooseGameplayTags(ObjectTypeTags);
			}
		);
		
	}

	if (ObjectTypeTags.Num() == 0)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("WARNING, Class %s doesn't have an ObjectType"),  *this->GetClass()->GetName()));

	DefaultMeshRelativeTransform = GetMesh()->GetRelativeTransform();
	DefaultGrabOffsetToMesh = GetMesh()->GetRelativeLocation() - GetGrabRelativeLocation();
	//AbilitySystemComponent->AddReplicatedLooseGameplayTags(ObjectTypeTags);
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
	if(MontageTag.MatchesTagExact(GameplayTags.Montage_Attack_LeftFoot))
	{
		return GetMesh()->GetSocketLocation(LeftFootSocketName);
	}
	if(MontageTag.MatchesTagExact(GameplayTags.Montage_Attack_RightFoot))
	{
		return GetMesh()->GetSocketLocation(RightFootSocketName);
	}
	if(MontageTag.MatchesTagExact(GameplayTags.Montage_Attack_UpperChest))
	{
		return GetMesh()->GetSocketLocation(UpperChestSocketName);
	}

	if(MontageTag.MatchesTagExact(GameplayTags.Montage_Attack_Head))
	{
		return GetMesh()->GetSocketLocation(HeadSocketName);
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

void AarpgCharacterBase::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>> &Abilities, bool ActivateImmediately)
{
	if(!HasAuthority()) return;

	UarpgAbilitySystemComponent* ArpgASC = CastChecked<UarpgAbilitySystemComponent>(AbilitySystemComponent);

	ArpgASC->AddCharacterAbilities(Abilities, ActivateImmediately);
}

void AarpgCharacterBase::RemoveCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>> &Abilities)
{
	if(!HasAuthority()) return;

	UarpgAbilitySystemComponent* ArpgASC = CastChecked<UarpgAbilitySystemComponent>(AbilitySystemComponent);

	ArpgASC->RemoveCharacterAbilities(Abilities);
}

void AarpgCharacterBase::ClearMovementRestrictions()
{
	FGameplayTagContainer Restrictions;
	Restrictions.AddTag(FArpgGameplayTags::Get().Player_Block_InputPressed);
	Restrictions.AddTag(FArpgGameplayTags::Get().Player_Block_InputReleased);
	Restrictions.AddTag(FArpgGameplayTags::Get().Player_Block_InputHeld);
	Restrictions.AddTag(FArpgGameplayTags::Get().Player_Block_Rotation);
	
	AbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(Restrictions);

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
}

void AarpgCharacterBase::SetIsTraversing(const bool bInIsTraversing, const FTransform InTarget)
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

