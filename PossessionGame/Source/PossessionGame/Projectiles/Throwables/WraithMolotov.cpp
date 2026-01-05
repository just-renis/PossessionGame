#include "WraithMolotov.h"
#include "NiagaraComponent.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Utilities/Data Utilities/FNiagaraAttribute.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"

void AWraithMolotov::Disable(const bool bHide)
{
	if (RadiusVfx) RadiusVfx->DeactivateImmediate();
	Super::Disable(bHide);
}

void AWraithMolotov::OnImpact(const FHitResult& Hit)
{
	Super::OnImpact(Hit);
	TMap<FName, FNiagaraAttribute> Attributes;
	const float RadiusIncrease = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetUpgradeAttributeValues(EUpgrade::WraithAbility1, EUpgradeAttribute::Size, true);
	const float CollisionRadius = DefaultRadius + DefaultRadius * RadiusIncrease * 0.01f;
	Attributes.Add(FName("Mesh Uniform Scale"), FNiagaraAttribute(true, CollisionRadius / DefaultRadius));
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->AttachVfx(EVfx::WraithMolotovRadius, nullptr, &RadiusVfx, Mesh, NAME_None,
	false, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false, EVariation::Default, Attributes);
}

void AWraithMolotov::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (FMath::IsNearlyEqual(Hit.ImpactNormal.Z, 1.f, 0.1f)) OnImpact(Hit);
}
