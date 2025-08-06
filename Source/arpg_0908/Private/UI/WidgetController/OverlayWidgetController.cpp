// Copyright Tinzie Games


#include "UI/WidgetController/OverlayWidgetController.h"

#include "ArpgGameplayTags.h"
#include "AbilitySystem/arpgAbilitySystemComponent.h"
#include "AbilitySystem/arpgAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	const UarpgAttributeSet* arpgAttributeSet = CastChecked<UarpgAttributeSet>(AttributeSet);
	
	OnHealthChanged.Broadcast(arpgAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(arpgAttributeSet->GetMaxHealth());
	
	OnManaChanged.Broadcast(arpgAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(arpgAttributeSet->GetMaxMana());

	if (UarpgAbilitySystemComponent* ArpgASC = Cast<UarpgAbilitySystemComponent>(AbilitySystemComponent))
	{
		if (ArpgASC->bStartupAbilitiesGiven)
		{
			UpdateOwnedAbilities(ArpgASC);
		}
	}

}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	const UarpgAttributeSet* arpgAttributeSet = CastChecked<UarpgAttributeSet>(AttributeSet);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(arpgAttributeSet->GetHealthAttribute()).AddLambda(
	[this](const FOnAttributeChangeData& Data)
		{
			OnHealthChanged.Broadcast(Data.NewValue);
		}
	);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(arpgAttributeSet->GetMaxHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnMaxHealthChanged.Broadcast(Data.NewValue);
		}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(arpgAttributeSet->GetManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnManaChanged.Broadcast(Data.NewValue);
		}
	);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(arpgAttributeSet->GetMaxManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnMaxManaChanged.Broadcast(Data.NewValue);
		}
	);

	if (UarpgAbilitySystemComponent* ArpgASC = Cast<UarpgAbilitySystemComponent>(AbilitySystemComponent))
	{
		if (ArpgASC->bStartupAbilitiesGiven)
		{
			UpdateOwnedAbilities(ArpgASC);	
		}
		else
		{
			ArpgASC->AbilitiesChanged.AddUObject(this, &UOverlayWidgetController::UpdateOwnedAbilities);
		}
		

		ArpgASC->EffectAssetTags.AddLambda(
			[this](const FGameplayTagContainer& AssetTags)
			{
				for(const FGameplayTag& Tag : AssetTags)
				{

					FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
					if(Tag.MatchesTag(MessageTag))
					{
						const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
						MessageWidgetRowDelegate.Broadcast(*Row);
					}
				}
			}
		);

		AbilitySystemComponent->RegisterGameplayTagEvent(FArpgGameplayTags::Get().Disable_Weapon_BasicAttack, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UOverlayWidgetController::DisableReceived);
		AbilitySystemComponent->RegisterGameplayTagEvent(FArpgGameplayTags::Get().Disable_Weapon_SecondaryAttack, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UOverlayWidgetController::DisableReceived);
		AbilitySystemComponent->RegisterGameplayTagEvent(FArpgGameplayTags::Get().Disable_Head_Ability1, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UOverlayWidgetController::DisableReceived);
		AbilitySystemComponent->RegisterGameplayTagEvent(FArpgGameplayTags::Get().Disable_Head_Ability2, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UOverlayWidgetController::DisableReceived);
		AbilitySystemComponent->RegisterGameplayTagEvent(FArpgGameplayTags::Get().Disable_Head_Ability3, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UOverlayWidgetController::DisableReceived);
		AbilitySystemComponent->RegisterGameplayTagEvent(FArpgGameplayTags::Get().Disable_Utility_Ability1, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UOverlayWidgetController::DisableReceived);
		AbilitySystemComponent->RegisterGameplayTagEvent(FArpgGameplayTags::Get().Disable_Utility_Ability2, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UOverlayWidgetController::DisableReceived);
	}
}

void UOverlayWidgetController::DisableReceived(const FGameplayTag CallbackTag, int32 NewCount)
{
	
	AbilityDisabledDelegate.Broadcast(CallbackTag, (NewCount > 0));
}

void UOverlayWidgetController::UpdateOwnedAbilities(UarpgAbilitySystemComponent* ArpgAbilitySystemComponent)
{
	if (!ArpgAbilitySystemComponent->bStartupAbilitiesGiven) return;

	//This is quite cool. We create a lambda function that constructs some ability info from an input spec and ASC.
	//We then bound it to "For Each Ability", so that each ability spec executes this function.
	FForEachAbility BroadcastDelegate;
	BroadcastDelegate.BindLambda([this, ArpgAbilitySystemComponent](const FGameplayAbilitySpec& AbilitySpec, bool MarkedForDeletion)
	{
		FArpgAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(ArpgAbilitySystemComponent->GetAbilityTagFromSpec(AbilitySpec));
		Info.InputTag = ArpgAbilitySystemComponent->GetInputTagFromSpec(AbilitySpec);
		Info.IsFlaggedForDeletion =  MarkedForDeletion;
		//Internally, this delegate broadcasts another delegate that widget components can bind to!
		AbilityInfoDelegate.Broadcast(Info);
	});
	
	ArpgAbilitySystemComponent->ForEachAbility(BroadcastDelegate);
}
