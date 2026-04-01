#include "SamuraiAnimationInstance.h"
#include "NiagaraComponent.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Weapons/Character Weapons/SamuraiWeapon.h"

void USamuraiAnimationInstance::PlaySlashVfx(const EVfx InVfxName, const int32 SlashIndex)
{
	const UWorld* World = GetWorld();
	if (!World) return;
	const UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance) return;
	const TObjectPtr<UPrimaryDataAssetsManager> DataAssetsManager = GameInstance->GetSubsystem<UPrimaryDataAssetsManager>();
	if (DataAssetsManager->GetVfxData(InVfxName).GetIsInLoad() && !DataAssetsManager->GetVfxData(InVfxName).GetIsLoaded()) return;
	switch (SlashIndex)
	{
		case 1:
			if (!FirstSlashVfx) DataAssetsManager->AttachVfx(InVfxName, [this]() -> bool { return Proprietor->IsAsyncAssetStillAllowedToBeUsed(); }, &FirstSlashVfx, Proprietor->GetMesh(), FName("FirstSlashSocket"));
			else FirstSlashVfx->Activate(true);
			break;
		case 2:
			if (!SecondSlashVfx) DataAssetsManager->AttachVfx(InVfxName, [this]() -> bool { return Proprietor->IsAsyncAssetStillAllowedToBeUsed(); }, &SecondSlashVfx, Proprietor->GetMesh(), FName("SecondSlashSocket"));
			else SecondSlashVfx->Activate(true);
			break;
		default: ;
	}
}

void USamuraiAnimationInstance::OnMontageEnd(UAnimMontage* InMontage, bool bIsInterrupted)
{
	Super::OnMontageEnd(InMontage, bIsInterrupted);
	if (!IsMontage(EEnemyAnimState::Slash, InMontage)) return;
	Proprietor->SetIsAnimActionInMotion(false);
	Cast<ASamuraiWeapon>(Proprietor->GetWeapon())->AI_StopProcessingMeleeAttack();
}
