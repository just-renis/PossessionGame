#include "GuardianAbility1.h"
#include "PossessionGame/AI/Enemies/Characters/Guardian.h"
#include "PossessionGame/Data Assets/Base Data Assets/AbilityData.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Enums/EUpgradeAttribute.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"
#include "PossessionGame/Weapons/Character Weapons/GuardianWeapon.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void UGuardianAbility1::AI_Execute()
{
	Proprietor->SetFirstAbilityUsageState(true);
	const float ReductionIncrease = GetWorld()->GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetUpgradeAttributeValues(Data->GetAbilityName(), EUpgradeAttribute::ReduceDamageTaken, IsProprietorPossessed());
	TotalDamageReduction = Data->GetBaseDamageReduction() / (1.f + ReductionIncrease * 0.01f);
	Proprietor->SetDamageReduction(TotalDamageReduction);
	Proprietor->PlaySound(ESound::GuardianOverChargeStart);
	Proprietor->AI_ToggleShieldVfx(true);
	Super::AI_Execute();
}

void UGuardianAbility1::AI_StopAbility()
{
	if (!bIsAbilityInDuration) return;
	Proprietor->SetFirstAbilityUsageState(false);
	Proprietor->SetDamageReduction(1.f);
	Proprietor->PlaySound(ESound::GuardianOverChargeEnd);
	Proprietor->AI_ToggleShieldVfx(false);
	Super::AI_StopAbility();
}

void UGuardianAbility1::P_Execute()
{
	Proprietor->SetFirstAbilityUsageState(true);
	Proprietor->SetIsActionInMotion(true);
	Proprietor->GetWeapon()->GetHandsAnimInstance()->PlayMontage(EHandsAnimState::OverCharge);
	Proprietor->P_SetFov(95.f);
	Cast<AGuardianWeapon>(Proprietor->GetWeapon())->ToggleHandsMeshAbilityState(true);
	const float ReductionIncrease = GetWorld()->GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetUpgradeAttributeValues(Data->GetAbilityName(), EUpgradeAttribute::ReduceDamageTaken, IsProprietorPossessed());
	TotalDamageReduction = Data->GetBaseDamageReduction() / (1.f + ReductionIncrease * 0.01f);
	Proprietor->SetDamageReduction(TotalDamageReduction);
	GetWorld()->GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::GuardianOverChargeStart);
	Proprietor->ToggleShield(true);
	if (bReflectDamageEnabled) Proprietor->SetReflectDamageState(true);
	Super::P_Execute();
}

void UGuardianAbility1::P_StopAbility()
{
	if (!bIsAbilityInDuration) return;
	Proprietor->SetFirstAbilityUsageState(false);
	Proprietor->P_SetFov(90.f);
	Proprietor->SetDamageReduction(1.f);
	Proprietor->ToggleShield(false);
	Cast<AGuardianWeapon>(Proprietor->GetWeapon())->ToggleHandsMeshAbilityState(false);
	GetWorld()->GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::GuardianOverChargeEnd);
	if (bReflectDamageEnabled) Proprietor->SetReflectDamageState(false);
	Super::P_StopAbility();
}

bool UGuardianAbility1::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void UGuardianAbility1::SetProprietor(TObjectPtr<AEnemy> AbilityProprietor) { Proprietor = Cast<AGuardian>(AbilityProprietor); }

void UGuardianAbility1::SetupUpgradeAppliance(const bool bIsPlayer)
{
	bReflectDamageEnabled = DelegateManager->RequestSpecialUpgradeAvailability.Execute(EUpgrade::GuardianAbility1, EUpgradeAttribute::Special1);
	Super::SetupUpgradeAppliance(bIsPlayer);
}
