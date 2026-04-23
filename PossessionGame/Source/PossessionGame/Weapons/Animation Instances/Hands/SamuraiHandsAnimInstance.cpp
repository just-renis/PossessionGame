#include "SamuraiHandsAnimInstance.h"
#include "NiagaraComponent.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

void USamuraiHandsAnimInstance::PlaySlashVfx()
{
	if (!SlashVfx) AssetsManager->AttachVfx(EVfx::PlayerSamuraiSlash, [this]() -> bool { return Proprietor->IsAsyncAssetStillAllowedToBeUsed(); }, &SlashVfx, GetSkelMeshComponent(), FName("RootSocket"));
	else SlashVfx->Activate(true);
}
