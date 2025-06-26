// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "arpgCharacterBase.generated.h"

class UArpgTemporaryTextComponent;
class UDebuffNiagaraComponent;
class UGameplayAbility;
class UGameplayEffect;
class UAbilitySystemComponent;
class UAttributeSet;
class UAnimMontage;

UCLASS(Abstract)
class ARPG_0908_API AarpgCharacterBase : public ACharacter, public IAbilitySystemInterface, public ICombatInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AarpgCharacterBase();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const { return AttributeSet; };

	//Combat Interface overrides
	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag MontageTag) override;
	virtual bool IsDead_Implementation() const override;
	virtual AActor* GetAvatar_Implementation() override;
	virtual UAnimMontage* GetHitReactMontage_Implementation() override;
	virtual void Die(const FVector& DeathImpulse) override;
	virtual TArray<FTaggedMontage> GetAttackMontages_Implementation() override;
	virtual FOnASCRegistered& GetOnASCRegisteredDelegate() override;
	virtual FOnDeathSignature& GetOnDeathDelegate() override;
	virtual USkeletalMeshComponent* GetWeapon_Implementation() override;
	virtual bool IsInPlay_Implementation() const override;

	FOnASCRegistered OnASCRegistered;
	FOnDeathSignature OnDeath;

	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleDeath(const FVector& DeathImpulse);

	

	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FTaggedMontage> AttackMontages;

	UFUNCTION(BlueprintCallable)
	FTransform GetTraversalTargetTransform() const {return TraversalTarget;};

	UFUNCTION(BlueprintCallable)
	void SetIsTraversing(const bool& bInIsTraversing, const FTransform InTarget = FTransform());
	
	UFUNCTION(BlueprintCallable)
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>> &Abilities, bool ActivateImmediately = false);
	
	void RemoveCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>> &Abilities);
	
	UPROPERTY(ReplicatedUsing=OnRep_Stunned, BlueprintReadOnly)
	bool bIsStunned = false;

	UPROPERTY(ReplicatedUsing=OnRep_Burned, BlueprintReadOnly)
	bool bIsBurning = false;

	UFUNCTION()
	virtual void Onrep_Stunned();

	UFUNCTION()
	virtual void Onrep_Burned();
	
	UFUNCTION(BlueprintCallable)
	void SetCapsulePawnCollisionEnabled(const bool Enabled);
	
	UFUNCTION(BlueprintCallable)
	void SetInPlay(const bool InIsInPlay);

	UFUNCTION(BlueprintImplementableEvent)
	void OnCustomSpawned();

	
	///Ability Callable functions for specific functionality.
	//SnappingDuration is an optional parameter. Leave negative for unlimited snapping duration (must be cancelled manually by the server)
	UFUNCTION(BlueprintCallable, Category = "AbilityCallable")
	void ServerSetClientBeginSnapToTargetSocket(AarpgCharacterBase *Target, const FName SocketName, const float SnappingDuration = -1.0f);
	
	UFUNCTION(BlueprintCallable, Category = "AbilityCallable")
	void ServerSetClientEndSnapToTargetSocket();

	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastBeginSnapToTargetSocket(AarpgCharacterBase *Target, const FName SocketName, const float SnappingDuration = -1.0f);
	
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastEndSnapToTargetSocket();
	
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn")
	TSubclassOf<UGameplayAbility> SpawnAnimationAbility;

	//All of the head logic is unique to the playable characters. Enemies should just create a mesh and use it however they please in blueprints.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<USkeletalMeshComponent> BaseHeadMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;

	
	UPROPERTY(EditAnywhere, Category = "Combat")
	FName WeaponTipSocketName;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	FName LeftHandSocketName;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName RightHandSocketName;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	FName LeftFootSocketName;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName RightFootSocketName;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName UpperChestSocketName;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	FName HeadSocketName;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName CustomOrDebugSocketName;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName TraceBeginSocketName;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	FName TraceEndSocketName;
	
	bool bDead = false;
	
	bool bInPlay = false;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	virtual void InitAbilityActorInfo();
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultVitalAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultDerivedAttributes;

	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const;
	
	virtual void InitializeDefaultAttributes() const;

	
	FTransform TraversalTarget;
	bool bIsTraversing = false; //While Traversing, cannot use normal movement/input.

	//Dissolve Effects

	void Dissolve();

	UFUNCTION(BlueprintImplementableEvent)
	void StartDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);

	UFUNCTION(BlueprintImplementableEvent)
	void StartWeaponDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> DissolveMaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> WeaponDissolveMaterialInstance;
	
	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UDebuffNiagaraComponent> BurnNiagaraComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UDebuffNiagaraComponent> StunNiagaraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float BaseWalkSpeed = 250.f;
	
	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UArpgTemporaryTextComponent> SpeechTextComponentClass;
	
private:

	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;

	FTimerHandle SnapTimerHandle;
	FTimerHandle OptionalSnapDurationHandle;



	///DEBUG VARIABLES
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TestScene")
	float WaitTimeBetweenTestAbilityCasts = 1.0f;
protected:

	
	UFUNCTION(BlueprintCallable)
	TSubclassOf<UGameplayAbility> GetFirstStartupAbility() const {return StartupAbilities[0];};
};
