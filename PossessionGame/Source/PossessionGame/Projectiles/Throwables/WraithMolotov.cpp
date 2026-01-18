#include "WraithMolotov.h"
#include "Components/AudioComponent.h"
#include "PossessionGame/Data Assets/Base Data Assets/ThrowableData.h"
#include "PossessionGame/Enums/EDamageReason.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/OvertimeDamagers/OvertimeDamager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"

AWraithMolotov::AWraithMolotov()
{
	DeploySfx = CreateDefaultSubobject<UAudioComponent>(TEXT("ImpactSfx"));
	DeploySfx->SetAutoActivate(false);
	DeploySfx->SetupAttachment(Mesh);
	Sounds.Add(ESound::WraithMolotovImpact, DeploySfx);
}

void AWraithMolotov::OnImpact(const FHitResult& InHit)
{
	Super::OnImpact(InHit);
	PlaySound(ESound::WraithMolotovImpact);
	const TObjectPtr<AOvertimeDamager> BurnArea = Cast<AOvertimeDamager>(GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(EPoolObject::WraithMolotovArea, InHit.Location + FVector::UpVector, InHit.Normal.Rotation()));
	BurnArea->Init();
	BurnArea->Setup(true, Data->GetDamageOverTimeData(), EDamageReason::WraithMolotovDamage);
	GetWorld()->GetSubsystem<UPoolingManager>()->ReturnObjectToPool(EPoolObject::WraithMolotov, this);
}

void AWraithMolotov::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (FMath::IsNearlyEqual(Hit.ImpactNormal.Z, 1.f, 0.1f)) OnImpact(Hit);
}
