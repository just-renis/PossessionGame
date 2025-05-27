#include "GuardianAbility1.h"
#include "PossessionGame/AI/Enemies/Characters/Guardian.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Enums/EPostProcessMaterial.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/Utilities/Subsystems/PostProcessManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void UGuardianAbility1::P_Execute()
{
	Proprietor->SetActionInMotion(true);
	Proprietor->GetWeapon()->GetHandsAnimInstance()->PlayMontage(EHandsAnimState::OverCharge);
	Proprietor->P_SetFov(95.f);
	Proprietor->SetDamageReduction(0.8f);
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