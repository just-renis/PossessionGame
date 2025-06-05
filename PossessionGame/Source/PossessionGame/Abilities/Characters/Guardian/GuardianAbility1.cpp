#include "GuardianAbility1.h"
#include "PossessionGame/AI/Enemies/Characters/Guardian.h"
#include "PossessionGame/Data Assets/Base Data Assets/AbilityData.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Enums/EPostProcessMaterial.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/Enums/EUpgradeAttribute.h"
#include "PossessionGame/Utilities/Subsystems/PostProcessManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/UpgradeManager.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void UGuardianAbility1::P_Execute()
{
	Proprietor->SetActionInMotion(true);
	Proprietor->GetWeapon()->GetHandsAnimInstance()->PlayMontage(EHandsAnimState::OverCharge);
	Proprietor->P_SetFov(95.f);
	const float ReductionIncrease = GetWorld()->GetSubsystem<UUpgradeManager>()->GetUpgradeAttributeValue(IsProprietorPossessed(), Data->GetAbilityName(), EUpgradeAttribute::ReduceDamageTaken);
	TotalDamageReduction = BaseDamageReduction / (1.f + ReductionIncrease * 0.01f);
	Proprietor->SetDamageReduction(TotalDamageReduction);
	GetWorld()->GetSubsystem<UPostProcessManager>()->ToggleMaterial(EPostProcessMaterial::GuardianOverCharge, 1.f);
	GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::GuardianOverChargeStart);
	Proprietor->TriggerShield(true);
	bIsAbilityInDuration = true;
	Super::P_Execute();
}

void UGuardianAbility1::P_StopAbility()
{
	Proprietor->P_SetFov(90.f);
	Proprietor->SetDamageReduction(1.f);
	Proprietor->TriggerShield(false);
	GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::GuardianOverChargeEnd);
	GetWorld()->GetSubsystem<UPostProcessManager>()->ToggleMaterial(EPostProcessMaterial::GuardianOverCharge, 0.f);
	Super::P_StopAbility();
}

bool UGuardianAbility1::IsProprietorPossessed() { return Proprietor->GetIsPossessed(); }

void UGuardianAbility1::SetProprietor(TObjectPtr<AEnemy> AbilityProprietor) { Proprietor = Cast<AGuardian>(AbilityProprietor); }