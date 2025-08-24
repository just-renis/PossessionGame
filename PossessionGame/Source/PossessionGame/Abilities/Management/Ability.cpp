#include "Ability.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Data Assets/Base Data Assets/AbilityData.h"
#include "PossessionGame/Enums/EUpgradeAttribute.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"

EAbility UAbility::GetAbilityName() const { return Data->GetAbilityName(); }

void UAbility::P_InitiateExecute()
{
	if (bIsAbilityInDuration) P_StopAbility();
	P_Execute();
}

void UAbility::P_StopAbility()
{
	ResetDuration();
	(void)DelegateManager->OnAbilityEnd.ExecuteIfBound(AbilityIndex, GetIsAbilityCharged());
}

void UAbility::Tick(const float InDeltaTime)
{
	if (bIsAbilityInDuration) AbilityInDurationLogic(InDeltaTime);
	if (!bIsCooldownKill && CurrentChargesAmount < MaxChargesAmount) CooldownLogic(InDeltaTime);
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

void UAbility::P_Execute()
{
	UseCharge();
	(void)DelegateManager->OnAbilityUse.ExecuteIfBound(AbilityIndex, GetIsAbilityCharged(), !bIsAbilityInDuration);
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
	const TObjectPtr<URunSaveSlot> RunSaveSlot = GetWorld()->GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot();
	const float CooldownReduction = RunSaveSlot->GetUpgradeAttributeValues(Data->GetAbilityName(), EUpgradeAttribute::AbilityCooldownReduction, bIsPlayer);
	ReqCooldownSecs = Data->GetCooldownSecs() / (1.f + CooldownReduction * 0.01f);
	MaxChargesAmount = Data->GetChargesAmount() + RunSaveSlot->GetUpgradeAttributeValues(Data->GetAbilityName(), EUpgradeAttribute::AbilityChargesAmount, bIsPlayer);
	if (Data->GetIsAbilityWithDuration())
	{
		DurationTime = Data->GetAbilityDuration() + Data->GetAbilityDuration() * RunSaveSlot->GetUpgradeAttributeValues(Data->GetAbilityName(), EUpgradeAttribute::AbilityDuration, bIsPlayer) * 0.01f;
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
