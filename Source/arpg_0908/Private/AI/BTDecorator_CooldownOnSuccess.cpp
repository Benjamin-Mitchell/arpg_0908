// Copyright Tinzie Games


#include "AI/BTDecorator_CooldownOnSuccess.h"

UBTDecorator_CooldownOnSuccess::UBTDecorator_CooldownOnSuccess()
{
	NodeName = "Cooldown On Success";
	INIT_DECORATOR_NODE_NOTIFY_FLAGS();
	CoolDownTime = 5.0f;
	
	// aborting child nodes doesn't makes sense, cooldown starts after leaving this branch
	bAllowAbortChildNodes = false;
}

bool UBTDecorator_CooldownOnSuccess::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
                                                                uint8* NodeMemory) const
{
	FBTCooldownDecoratorMemory* DecoratorMemory = CastInstanceNodeMemory<FBTCooldownDecoratorMemory>(NodeMemory);
	const double RecalcTime = (OwnerComp.GetWorld()->GetTimeSeconds() - CoolDownTime.GetValue(OwnerComp));
	return RecalcTime >= DecoratorMemory->LastUseTimestamp;
}

void UBTDecorator_CooldownOnSuccess::OnNodeDeactivation(FBehaviorTreeSearchData& SearchData,
	EBTNodeResult::Type NodeResult)
{
	if (NodeResult == EBTNodeResult::Succeeded)
	{
		FBTCooldownDecoratorMemory* DecoratorMemory = GetNodeMemory<FBTCooldownDecoratorMemory>(SearchData);
		DecoratorMemory->LastUseTimestamp = SearchData.OwnerComp.GetWorld()->GetTimeSeconds();
		DecoratorMemory->bRequestedRestart = false;
	}
}

void UBTDecorator_CooldownOnSuccess::InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTMemoryInit::Type InitType) const
{
	FBTCooldownDecoratorMemory* DecoratorMemory = InitializeNodeMemory<FBTCooldownDecoratorMemory>(NodeMemory, InitType);
	if (InitType == EBTMemoryInit::Initialize)
	{
		DecoratorMemory->LastUseTimestamp = TNumericLimits<double>::Lowest();
	}

	DecoratorMemory->bRequestedRestart = false;
}

void UBTDecorator_CooldownOnSuccess::CleanupMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTMemoryClear::Type CleanupType) const
{
	CleanupNodeMemory<FBTCooldownDecoratorMemory>(NodeMemory, CleanupType);
}

uint16 UBTDecorator_CooldownOnSuccess::GetInstanceMemorySize() const
{
	return sizeof(FBTCooldownDecoratorMemory);
}

FString UBTDecorator_CooldownOnSuccess::GetStaticDescription() const
{
	// basic info: result after time
	return FString::Printf(TEXT("%s: lock for %s s after successful children execution and return %s"), *Super::GetStaticDescription(),
		*CoolDownTime.ToString(), *UBehaviorTreeTypes::DescribeNodeResult(EBTNodeResult::Failed));
}

void UBTDecorator_CooldownOnSuccess::DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
                                                           EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const
{
	Super::DescribeRuntimeValues(OwnerComp, NodeMemory, Verbosity, Values);

	FBTCooldownDecoratorMemory* DecoratorMemory = CastInstanceNodeMemory<FBTCooldownDecoratorMemory>(NodeMemory);
	const double TimePassed = OwnerComp.GetWorld()->GetTimeSeconds() - DecoratorMemory->LastUseTimestamp;
	
	if (TimePassed < CoolDownTime.GetValue(OwnerComp))
	{
		Values.Add(FString::Printf(TEXT("%s in %ss"),
			(FlowAbortMode == EBTFlowAbortMode::None) ? TEXT("unlock") : TEXT("restart"),
			*FString::SanitizeFloat(CoolDownTime.GetValue(OwnerComp) - TimePassed)));
	}
}

void UBTDecorator_CooldownOnSuccess::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FBTCooldownDecoratorMemory* DecoratorMemory = CastInstanceNodeMemory<FBTCooldownDecoratorMemory>(NodeMemory);
	if (!DecoratorMemory->bRequestedRestart)
	{
		const double RecalcTime = (OwnerComp.GetWorld()->GetTimeSeconds() - CoolDownTime.GetValue(OwnerComp));
		if (RecalcTime >= DecoratorMemory->LastUseTimestamp)
		{
			DecoratorMemory->bRequestedRestart = true;
			OwnerComp.RequestExecution(this);
		}
	}
}
