#include "WraithMolotov.h"
#include "NiagaraComponent.h"
#include "PossessionGame/Data Assets/Base Data Assets/ThrowableData.h"
#include "PossessionGame/Enums/EDamageReason.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/OvertimeDamagers/OvertimeDamager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"

void AWraithMolotov::Disable(const bool bHide)
{
	if (RadiusVfx) RadiusVfx->DeactivateImmediate();
	Super::Disable(bHide);
}

void AWraithMolotov::OnImpact(const FHitResult& Hit)
{
	Super::OnImpact(Hit);
	const TObjectPtr<AOvertimeDamager> BurnArea = Cast<AOvertimeDamager>(GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(EPoolObject::WraithMolotovArea, Hit.Location, Hit.Normal.Rotation()));
	BurnArea->Init();
	BurnArea->Setup(true, Data->GetDamage(), Data->GetOvertimeTickTrigger(), Data->GetOvertimeAreaLifeSpan(), Data->GetOvertimeAreaSize(), EDamageReason::WraithMolotovDamage);
	GetWorld()->GetSubsystem<UPoolingManager>()->ReturnObjectToPool(EPoolObject::WraithMolotov, this);
}

void AWraithMolotov::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (FMath::IsNearlyEqual(Hit.ImpactNormal.Z, 1.f, 0.1f)) OnImpact(Hit);
}
