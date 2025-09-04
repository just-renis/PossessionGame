#include "RangerAbility1.h"
#include "PossessionGame/AI/Enemies/Characters/Ranger.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Enums/EVariation.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PlayerDataManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"
#include "PossessionGame/Utilities/Subsystems/VariationManager.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void URangerAbility1::AI_Execute()
{
	Proprietor->SetIsActionInMotion(true);
	Proprietor->GetAnimInstance()->PlayMontage(EEnemyAnimState::GrenadeLaunch);
	Proprietor->PlaySound(ESound::RangerGrenadeLaunchAim, true);
	Super::AI_Execute();
}

void URangerAbility1::P_Execute()
{
	Proprietor->SetIsActionInMotion(true);
	Proprietor->GetWeapon()->GetHandsAnimInstance()->PlayMontage(EHandsAnimState::GrenadeLaunch);
	Proprietor->P_LaunchGrenade();
	if (bFirstSpecialUpgradeEnabled) GetWorld()->GetGameInstance()->GetSubsystem<UPlayerDataManager>()->AddRangerGrenadeSpecialStack();
	Super::P_Execute();
}

float URangerAbility1::AI_GetCooldownReduction()
{
	const float CooldownReduction = Super::AI_GetCooldownReduction();
	if (!GetWorld()->GetGameInstance()->GetSubsystem<UDelegateManager>()->RequestSpecialUpgradeAvailability.Execute(EUpgrade::RangerAbility1, EUpgradeAttribute::Special1)) return CooldownReduction;
	const int32 AbilitiesUpgradesLevel = GetWorld()->GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetCurrentLevelSumOfCharacterAbilities(Proprietor->GetCharacterName());
	return CooldownReduction + AbilitiesUpgradesLevel * 10.f;
}

void URangerAbility1::P_ApplyVariation()
{
	if (EVariation::RangerVariationBlue == Proprietor->GetWorld()->GetSubsystem<UVariationManager>()->GetCurrentVariation(Proprietor->GetCharacterName()))
	{
		MaxChargesAmount += 2;
		ReqCooldownSecs *= 0.7f;
	}
}

bool URangerAbility1::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void URangerAbility1::SetProprietor(TObjectPtr<AEnemy> InAbilityProprietor) { Proprietor = Cast<ARanger>(InAbilityProprietor); }

void URangerAbility1::CooldownLogic(const float InDeltaTime)
{
	Super::CooldownLogic(InDeltaTime);
	if (!Proprietor->GetIsHijacked() && CurrentChargesAmount > 0) Proprietor->AI_ToggleGrenadeTrigger(true);
}

void URangerAbility1::SetupUpgradeAppliance(const bool bIsPlayer)
{
	bFirstSpecialUpgradeEnabled = DelegateManager->RequestSpecialUpgradeAvailability.Execute(EUpgrade::RangerAbility1, EUpgradeAttribute::Special1);
	Super::SetupUpgradeAppliance(bIsPlayer);
}
