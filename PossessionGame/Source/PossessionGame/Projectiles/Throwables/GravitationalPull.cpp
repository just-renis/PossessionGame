#include "GravitationalPull.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Data Assets/Base Data Assets/ThrowableData.h"
#include "PossessionGame/Enums/EDamageReason.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/OvertimeDamagers/OvertimeDamager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

void AGravitationalPull::InitWithProprietor(const TObjectPtr<AEnemy> InProprietor)
{
	Super::InitWithProprietor(InProprietor);
	if (!BallVfx) return;
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->AttachVfx(EVfx::TankGravitationalBall, [this]() -> bool { return Mesh->GetVisibleFlag(); }, &BallVfx, Mesh, NAME_None, Proprietor->GetVariation());
}

void AGravitationalPull::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) { OnImpact(Hit); }

void AGravitationalPull::OnImpact(const FHitResult& InHit)
{
	Super::OnImpact(InHit);
	const TObjectPtr<AOvertimeDamager> GravitationalPullArea = Cast<AOvertimeDamager>(GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(EPoolObject::TankGravitationalPullArea, InHit.Location + FVector::UpVector, InHit.Normal.Rotation()));
	GravitationalPullArea->InitWithProprietor(Proprietor);
	GravitationalPullArea->Setup(true, Data->GetDamageOverTimeData(), EDamageReason::None);
	GetWorld()->GetSubsystem<UPoolingManager>()->ReturnObjectToPool(EPoolObject::TankGravitationalPull, this);
}
