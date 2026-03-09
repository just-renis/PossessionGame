#include "GravitationalPull.h"
#include "PossessionGame/Data Assets/Base Data Assets/ThrowableData.h"
#include "PossessionGame/Enums/EDamageReason.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/OvertimeDamagers/OvertimeDamager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

void AGravitationalPull::BeginPlay()
{
	Super::BeginPlay();
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->AttachVfx(EVfx::TankHandBall, [this]() -> bool {  UE_LOG(LogTemp, Error, TEXT("MESH FLAG: %i"), Mesh->GetVisibleFlag()); return Mesh->GetVisibleFlag(); },
	&BallVfx, Mesh, NAME_None, false, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTargetIncludingScale, false, EVariation::Default,
	TMap<FName, FNiagaraAttribute>({{FName("1"), FNiagaraAttribute(EUserParamType::Float, 0.f)}, {FName("2"), FNiagaraAttribute(EUserParamType::Float, 0.f)},
	{FName("3"), FNiagaraAttribute(EUserParamType::Float, 0.f)}}));
}

void AGravitationalPull::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (FMath::IsNearlyEqual(Hit.ImpactNormal.Z, 1.f, 0.1f)) OnImpact(Hit);
}

void AGravitationalPull::OnImpact(const FHitResult& InHit)
{
	Super::OnImpact(InHit);
	const TObjectPtr<AOvertimeDamager> GravitationalPullArea = Cast<AOvertimeDamager>(GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(EPoolObject::TankGravitationalPullArea, InHit.Location + FVector::UpVector, InHit.Normal.Rotation()));
	GravitationalPullArea->Init();
	GravitationalPullArea->Setup(true, Data->GetDamageOverTimeData(), EDamageReason::None);
	GetWorld()->GetSubsystem<UPoolingManager>()->ReturnObjectToPool(EPoolObject::TankGravitationalPull, this);
}
