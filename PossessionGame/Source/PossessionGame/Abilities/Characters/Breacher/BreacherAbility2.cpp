#include "BreacherAbility2.h"
#include "PossessionGame/AI/Enemies/Characters/Breacher.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "PossessionGame/Data Assets/Base Data Assets/AbilityData.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void UBreacherAbility2::AI_Execute()
{
	Proprietor->GetAnimInstance()->SetIsControlRigEnabled(false);
	Proprietor->GetAI()->SetControlRigAvailabilityToggle(false);
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

bool UBreacherAbility2::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void UBreacherAbility2::SetProprietor(TObjectPtr<AEnemy> AbilityProprietor) { Proprietor = Cast<ABreacher>(AbilityProprietor); }

void UBreacherAbility2::CooldownLogic(const float InDeltaTime)
{
	Super::CooldownLogic(InDeltaTime);
	if (!Proprietor->GetIsHijacked() && CurrentChargesAmount > 0) Proprietor->AI_EnableTriggeringBash();
}
