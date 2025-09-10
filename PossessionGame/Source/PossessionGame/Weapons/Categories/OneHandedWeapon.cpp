#include "OneHandedWeapon.h"

AOneHandedWeapon::AOneHandedWeapon()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("root"));
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("weaponMesh"));
	Mesh->SetupAttachment(RootComponent);
}

void AOneHandedWeapon::BeginPlay()
{
	Super::BeginPlay();
	Mesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	Mesh->SetComponentTickEnabled(false);
	Mesh->bEnableUpdateRateOptimizations = true;
	Mesh->SkipUpdateOverlapsOptimEnabled = true;
	Mesh->SetAllowAnimCurveEvaluation(false);
	Mesh->bUseRefPoseOnInitAnim = true;
	Mesh->SetComponentTickInterval(1.f / 30.f);
}
