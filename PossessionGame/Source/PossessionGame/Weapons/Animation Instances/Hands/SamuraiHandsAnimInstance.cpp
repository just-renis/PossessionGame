#include "SamuraiHandsAnimInstance.h"
#include "NiagaraComponent.h"
#include "PossessionGame/AI/Enemies/Characters/Samurai.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Weapons/Character Weapons/SamuraiWeapon.h"

void USamuraiHandsAnimInstance::PlaySlashVfx(const bool bIsFirstSlash)
{
	if (bIsFirstSlash)
	{
		if (!FirstSlashVfx) AssetsManager->AttachVfx(EVfx::PlayerSamuraiSlash, [this]() -> bool { return Proprietor->IsAsyncAssetStillAllowedToBeUsed(); }, &FirstSlashVfx, GetSkelMeshComponent(), FName("RootSocket"));
		else FirstSlashVfx->Activate(true);
	}
	else
	{
		if (!SecondSlashVfx) AssetsManager->AttachVfx(EVfx::PlayerSamuraiSlash, [this]() -> bool { return Proprietor->IsAsyncAssetStillAllowedToBeUsed(); }, &SecondSlashVfx, GetSkelMeshComponent(), FName("SlashSocket2"));
		else SecondSlashVfx->Activate(true);
	}
}

void USamuraiHandsAnimInstance::OnMontageEnd(UAnimMontage* InMontage, const bool bIsInterrupted)
{
	Super::OnMontageEnd(InMontage, bIsInterrupted);
	if (IsMontage(EHandsAnimState::QuickMelee, InMontage))
	{
		Cast<ASamuraiWeapon>(Proprietor->GetWeapon())->P_ChangeComboState(false);
		return;
	}
	if (!IsMontage(EHandsAnimState::DashSlash, InMontage)) return;
	Cast<ASamurai>(Proprietor)->P_EndDash();
}
