#include "Ability.h"

#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Data Assets/Base Data Assets/AbilityData.h"
#include "PossessionGame/Enums/EUpgradeAttribute.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/UpgradeManager.h"

EAbility UAbility::GetAbilityName() const { return Data->GetAbilityName(); }

int32 UAbility::GetAbilityIndex() const { return Data->GetAbilityIndex(); }

void UAbility::P_Execute()
{
	UseCharge();
	(void)DelegateManager->OnAbilityUse.ExecuteIfBound(GetAbilityIndex(), GetIsAbilityCharged(), !bIsAbilityInDuration);
}

void UAbility::P_StopAbility()
{
	ResetDuration();
	(void)DelegateManager->OnAbilityEnd.ExecuteIfBound(GetAbilityIndex(), GetIsAbilityCharged());
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
	if (InAbilityProprietor->GetIsPossessed()) P_ApplyVariation();
	SetupUpgradeAppliance(IsProprietorPossessed());
}

void UAbility::UseCharge()
{
	if (bIsCooldownKill) CurrentCooldownKill = 0;
	else CurrentChargesAmount--;
	if (bIsRefreshingOtherAbility) RefreshOtherAbility();
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
	const float CooldownReduction = GetWorld()->GetSubsystem<UUpgradeManager>()->GetUpgradeAttributeValue(bIsPlayer, Data->GetAbilityName(), EUpgradeAttribute::AbilityCooldownReduction);
	ReqCooldownSecs = Data->GetCooldownSecs() / (1.f + CooldownReduction * 0.01f);
	MaxChargesAmount = Data->GetChargesAmount() + GetWorld()->GetSubsystem<UUpgradeManager>()->GetUpgradeAttributeValue(bIsPlayer, Data->GetAbilityName(), EUpgradeAttribute::AbilityChargesAmount);
	if (Data->GetIsAbilityWithDuration())
	{
		DurationTime = Data->GetAbilityDuration() + Data->GetAbilityDuration() * GetWorld()->GetSubsystem<UUpgradeManager>()->GetUpgradeAttributeValue(bIsPlayer, Data->GetAbilityName(), EUpgradeAttribute::AbilityDuration) * 0.01f;
		CurrentDuration = DurationTime;
	}
}

void UAbility::SetupAdditionalProperties(TObjectPtr<UAbility> InAbilityToRefresh)
{
	if (Data->GetRefreshAmount() > 0.f) SetupRefreshOtherAbility(Data->GetRefreshAmount(), InAbilityToRefresh);
}

void UAbility::RefreshOtherAbility() const { if (!RefreshableAbility->GetIsAbilityInDuration()) RefreshableAbility->ReduceCooldownTimer(RefreshOtherAbilityAmount); }

void UAbility::SetupRefreshOtherAbility(const float InRefreshAmount, TObjectPtr<UAbility> InAbilityToRefresh)
{
	bIsRefreshingOtherAbility = true;
	RefreshOtherAbilityAmount = InRefreshAmount;
	RefreshableAbility = InAbilityToRefresh;
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
