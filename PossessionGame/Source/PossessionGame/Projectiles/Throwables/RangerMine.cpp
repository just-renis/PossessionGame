#include "RangerMine.h"
#include "NiagaraComponent.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

void ARangerMine::BeginPlay()
{
	Super::BeginPlay();
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->AttachVfx(EVfx::RangerMineRadius, [this]() -> bool { return bAlreadyLanded; }, &MineRadiusVfx, Mesh);
}

void ARangerMine::OnImpact(const FHitResult& Hit)
{
	Super::OnImpact(Hit);
	bAlreadyLanded = true;
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::RangerDeployMine);
	if (MineRadiusVfx) MineRadiusVfx->Activate();
}
