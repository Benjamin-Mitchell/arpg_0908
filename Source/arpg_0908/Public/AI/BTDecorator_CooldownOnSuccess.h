// Copyright Tinzie Games

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BehaviorTree/ValueOrBBKey.h"
#include "BTDecorator_CooldownOnSuccess.generated.h"

struct FBTCooldownDecoratorMemory
{
	double LastUseTimestamp;
	uint8 bRequestedRestart : 1;
};

/**
 * 
 */
UCLASS()
class ARPG_0908_API UBTDecorator_CooldownOnSuccess : public UBTDecorator
{
	GENERATED_BODY()


public:
	UBTDecorator_CooldownOnSuccess();

	UPROPERTY(Category=Decorator, EditAnywhere)
	FValueOrBBKey_Float CoolDownTime;
	
	UPROPERTY(Category=Decorator, EditAnywhere)
	bool InitializeOnCooldown;
	
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual void OnNodeDeactivation(FBehaviorTreeSearchData& SearchData, EBTNodeResult::Type NodeResult) override;

	virtual void InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryInit::Type InitType) const override;
	virtual void CleanupMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryClear::Type CleanupType) const override;

	virtual uint16 GetInstanceMemorySize() const override;
	virtual FString GetStaticDescription() const override;
	virtual void DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const override;

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
