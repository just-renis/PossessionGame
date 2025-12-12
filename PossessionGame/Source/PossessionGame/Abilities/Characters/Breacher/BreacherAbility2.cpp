#include "BreacherAbility2.h"
#include "PossessionGame/AI/Enemies/Animation Instances/BreacherAnimationInstance.h"
#include "PossessionGame/AI/Enemies/Characters/Breacher.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "PossessionGame/Data Assets/Base Data Assets/AbilityData.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void UBreacherAbility2::AI_Execute()
{
	Proprietor->GetAnimInstance()->SetIsControlRigEnabled(false);
	Proprietor->GetAI()->SetControlRigAvailabilityToggle(false);
	Cast<UBreacherAnimationInstance>(Proprietor->GetAnimInstance())->SetNonBlendableSlotState(true);
	Proprietor->GetAnimInstance()->PlayMontage(EEnemyAnimState::Bash);
	Super::AI_Execute();
}

void UBreacherAbility2::P_Execute()
{
	Proprietor->SetIsActionInMotion(true);
	Proprietor->GetWeapon()->GetHandsAnimInstance()->PlayMontageWithHold(EHandsAnimState::BreacherBash, 0.5f);
	const float ImpulseUpgrade = GetWorld()->GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetUpgradeAttributeValues(Data->GetAbilityName(), EUpgradeAttribute::Impulse, true);
	const float ImpulseStrength = Data->GetImpulseStrength() * (1.f + ImpulseUpgrade * 0.01f);
	Proprietor->P_Bash(ImpulseStrength);
	Super::P_Execute();
}

float UBreacherAbility2::AI_SpecialCooldownMultiplier()
{
	if (!DelegateManager->RequestSpecialUpgradeAvailability.Execute(EUpgrade::BreacherAbility2, EUpgradeAttribute::Special1)) return Super::AI_SpecialCooldownMultiplier();
	return 0.5f;
}

bool UBreacherAbility2::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void UBreacherAbility2::SetProprietor(TObjectPtr<AEnemy> AbilityProprietor) { Proprietor = Cast<ABreacher>(AbilityProprietor); }

void UBreacherAbility2::CooldownLogic(const float InDeltaTime)
{
	Super::CooldownLogic(InDeltaTime);
	if (!Proprietor->GetIsHijacked() && CurrentChargesAmount > 0) Proprietor->AI_EnableTriggeringBash();
}

void UBreacherAbility2::Setup(TObjectPtr<AEnemy> InAbilityOwner, TObjectPtr<UAbilityData> InAbilityData, TObjectPtr<UDelegateManager> InDelegateManager)
{
	Super::Setup(InAbilityOwner, InAbilityData, InDelegateManager);
	if (InAbilityOwner->GetIsHijacked()) DelegateManager->OnPlayerBashKill.BindUObject(this, &UBreacherAbility2::RechargeAbility);
}

void UBreacherAbility2::RechargeAbility()
{
	if (!DelegateManager->RequestSpecialUpgradeAvailability.Execute(EUpgrade::BreacherAbility2, EUpgradeAttribute::Special1)) return;
	CurrentChargesAmount++;
	CurrentCooldownSecs = 0.f;
}

void UBreacherAbility2::Destroy() { DelegateManager->OnPlayerBashKill.Unbind(); }
