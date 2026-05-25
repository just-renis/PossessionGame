#include "SamuraiHandsAnimInstance.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Weapons/Character Weapons/SamuraiWeapon.h"

void USamuraiHandsAnimInstance::PlaySlashVfx(const bool bIsFirstSlash)
{
	if (bIsFirstSlash)
	{
		if (!FirstSlashVfx) AssetsManager->AttachVfx(EVfx::PlayerSamuraiSlash, [this]() -> bool { return Proprietor->IsAsyncAssetStillAllowedToBeUsed(); }, &FirstSlashVfx, GetSkelMeshComponent(), FName("RootSocket"), GetProprietor()->GetVariation());
		else AssetsManager->ActivateWithVariation(EVfx::PlayerSamuraiSlash, nullptr, FirstSlashVfx, false, Proprietor->GetVariation());

	}
	else
	{
		if (!SecondSlashVfx) AssetsManager->AttachVfx(EVfx::PlayerSamuraiSlash, [this]() -> bool { return Proprietor->IsAsyncAssetStillAllowedToBeUsed(); }, &SecondSlashVfx, GetSkelMeshComponent(), FName("SlashSocket2"), GetProprietor()->GetVariation());
		else AssetsManager->ActivateWithVariation(EVfx::PlayerSamuraiSlash, nullptr, SecondSlashVfx, false, Proprietor->GetVariation());
	}
}

void USamuraiHandsAnimInstance::OnMontageEnd(UAnimMontage* InMontage, const bool bIsInterrupted)
{
	Super::OnMontageEnd(InMontage, bIsInterrupted);
	if (!IsMontage(EHandsAnimState::QuickMelee, InMontage)) return;
	Cast<ASamuraiWeapon>(Proprietor->GetWeapon())->P_ChangeComboState(false);
}
