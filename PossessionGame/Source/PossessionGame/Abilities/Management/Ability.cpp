#include "Ability.h"
#include "PossessionGame/Data Assets/Base Data Assets/AbilityData.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

void UAbility::Tick(float DeltaTime)
{
	if (bIsAbilityInDuration) AbilityInDurationLogic(DeltaTime);
	else if (!bIsCooldownKill && CurrentCooldownSecs > 0.f) CooldownLogic(DeltaTime);
}

void UAbility::Setup(TObjectPtr<AEnemy> AbilityProprietor, TObjectPtr<UAbilityData> AbilityData, TObjectPtr<UDelegateManager> InDelegateManager)
{
	DelegateManager = InDelegateManager;
	ReqCooldownSecs = AbilityData->GetCooldownSecs();
	ReqCooldownKill = AbilityData->GetCurrentCooldownKill();
	CurrentCooldownSecs = ReqCooldownSecs;
	CurrentCooldownKill = ReqCooldownKill;
	if (CurrentCooldownKill > 0) bIsCooldownKill = true;
	SetProprietor(AbilityProprietor);
}

void UAbility::ResetCooldown()
{
	if (bIsCooldownKill) CurrentCooldownKill = ReqCooldownKill;
	else CurrentCooldownSecs = ReqCooldownSecs;
	if (bIsRefreshingOtherAbility) RefreshOtherAbility();
}

void UAbility::CooldownLogic(const float DeltaTime) { CurrentCooldownSecs -= DeltaTime; }

void UAbility::SetupAdditionalProperties(TObjectPtr<UAbilityData> AbilityData, TObjectPtr<UAbility> AbilityToRefresh)
{
	if (AbilityData->GetRefreshAmount() > 0.f) SetupRefreshOtherAbility(AbilityData->GetRefreshAmount(), AbilityToRefresh);
	if (AbilityData->GetAbilityDuration() > 0.f) SetupDuration(AbilityData->GetAbilityDuration());
}

void UAbility::RefreshOtherAbility() { if (!RefreshableAbility->GetIsAbilityInDuration()) RefreshableAbility->ReduceCooldownTimer(RefreshOtherAbilityAmount); }

void UAbility::SetupDuration(float AbilityDuration)
{
	CurrentDuration = AbilityDuration;
	DurationTime = AbilityDuration;
}

void UAbility::SetupRefreshOtherAbility(float RefreshAmount, TObjectPtr<UAbility> AbilityToRefresh)
{
	bIsRefreshingOtherAbility = true;
	RefreshOtherAbilityAmount = RefreshAmount;
	RefreshableAbility = AbilityToRefresh;
}

void UAbility::ResetDuration()
{
	CurrentDuration = DurationTime;
	bIsAbilityInDuration = false;
}

void UAbility::AbilityInDurationLogic(float DeltaTime)
{
	CurrentDuration -= DeltaTime;
	if (CurrentDuration <= 0.f) IsProprietorPossessed() ? P_StopAbility() : AI_StopAbility();
}
