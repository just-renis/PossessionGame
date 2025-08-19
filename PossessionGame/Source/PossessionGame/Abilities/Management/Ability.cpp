#include "Ability.h"

#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Data Assets/Base Data Assets/AbilityData.h"
#include "PossessionGame/Enums/EUpgradeAttribute.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/UpgradeManager.h"

EAbility UAbility::GetAbilityName() const { return Data->GetAbilityName(); }

void UAbility::P_Execute()
{
	UseCharge();
	(void)DelegateManager->OnAbilityUse.ExecuteIfBound(AbilityIndex, GetIsAbilityCharged(), !bIsAbilityInDuration);
	(void)DelegateManager->OnAbilityUse.ExecuteIfBound(AbilityIndex, GetIsAbilityCharged(), !bIsAbilityInDuration);
}

void UAbility::P_StopAbility()
{
	ResetDuration();
	(void)DelegateManager->OnAbilityEnd.ExecuteIfBound(AbilityIndex, GetIsAbilityCharged());
}

void UAbility::Tick(const float InDeltaTime)
{
	if (bIsAbilityInDuration) AbilityInDurationLogic(InDeltaTime);
	else if (!bIsCooldownKill && CurrentChargesAmount < MaxChargesAmount) CooldownLogic(InDeltaTime);
}

void UAbility::Setup(TObjectPtr<AEnemy> InAbilityProprietor, TObjectPtr<UAbilityData> InAbilityData, TObjectPtr<UDelegateManager> InDelegateManager)
{
	Data = InAbilityData;
	DelegateManager = InDelegateManager;
	ReqCooldownKill = InAbilityData->GetCurrentCooldownKill();
	if (CurrentCooldownKill > 0) bIsCooldownKill = true;
	SetProprietor(InAbilityProprietor);
	SetupUpgradeAppliance(IsProprietorPossessed());
	if (InAbilityProprietor->GetIsHijacked()) P_ApplyVariation();
}

void UAbility::UseCharge()
{
	if (bIsCooldownKill) CurrentCooldownKill = 0;
	else CurrentChargesAmount--;
}

void UAbility::CooldownLogic(const float InDeltaTime)
{
	CurrentCooldownSecs += InDeltaTime;
	if (CurrentCooldownSecs < ReqCooldownSecs) return;
	CurrentChargesAmount++;
	CurrentCooldownSecs = 0.f;
}

void UAbility::SetupUpgradeAppliance(const bool bIsPlayer)
{
	const float CooldownReduction = 0.f; //GetWorld()->GetSubsystem<UUpgradeManager>()->GetUpgradeAttributeValue(bIsPlayer, Data->GetAbilityName(), EUpgradeAttribute::AbilityCooldownReduction);
	ReqCooldownSecs = Data->GetCooldownSecs() / (1.f + CooldownReduction * 0.01f);
	MaxChargesAmount = Data->GetChargesAmount(); //+ GetWorld()->GetSubsystem<UUpgradeManager>()->GetUpgradeAttributeValue(bIsPlayer, Data->GetAbilityName(), EUpgradeAttribute::AbilityChargesAmount);
	if (Data->GetIsAbilityWithDuration())
	{
		DurationTime = Data->GetAbilityDuration() + Data->GetAbilityDuration(); // * GetWorld()->GetSubsystem<UUpgradeManager>()->GetUpgradeAttributeValue(bIsPlayer, Data->GetAbilityName(), EUpgradeAttribute::AbilityDuration) * 0.01f;
		CurrentDuration = DurationTime;
	}
}

void UAbility::ResetDuration()
{
	CurrentDuration = DurationTime;
	bIsAbilityInDuration = false;
}

void UAbility::AbilityInDurationLogic(const float InDeltaTime)
{
	CurrentDuration -= InDeltaTime;
	if (CurrentDuration <= 0.f) IsProprietorPossessed() ? P_StopAbility() : AI_StopAbility();
}
