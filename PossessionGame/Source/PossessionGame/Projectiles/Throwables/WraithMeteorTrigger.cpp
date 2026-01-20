#include "WraithMeteorTrigger.h"
#include "Components/AudioComponent.h"
#include "PossessionGame/Data Assets/Base Data Assets/ThrowableData.h"
#include "PossessionGame/Enums/EDamageReason.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/OvertimeDamagers/OvertimeDamager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"

AWraithMeteorTrigger::AWraithMeteorTrigger()
{
	DeploySfx = CreateDefaultSubobject<UAudioComponent>(TEXT("ImpactSfx"));
	DeploySfx->SetAutoActivate(false);
	DeploySfx->SetupAttachment(Mesh);
	Sounds.Add(ESound::WraithMeteorTriggerImpact, DeploySfx);
}

void AWraithMeteorTrigger::BeginPlay()
{
	Super::BeginPlay();
	if (!Mesh->GetMaterial(0)) return;
	MI_MeshBaseMaterial = Mesh->CreateAndSetMaterialInstanceDynamic(0);
	MI_MeshBaseMaterial->SetVectorParameterValue("Color", FLinearColor(0.571f, 0.f, 0.f));
}

void AWraithMeteorTrigger::OnImpact(const FHitResult& InHit)
{
	Super::OnImpact(InHit);
	PlaySound(ESound::WraithMeteorTriggerImpact);
	const TObjectPtr<AOvertimeDamager> MeteorShowerArea = Cast<AOvertimeDamager>(GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(EPoolObject::WraithMeteorShowerArea, InHit.Location + FVector::UpVector, InHit.Normal.Rotation()));
	MeteorShowerArea->Init(Proprietor);
	MeteorShowerArea->Setup(true, Data->GetDamageOverTimeData(), EDamageReason::None);
	GetWorld()->GetSubsystem<UPoolingManager>()->ReturnObjectToPool(EPoolObject::WraithMeteorTrigger, this);
}

void AWraithMeteorTrigger::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (FMath::IsNearlyEqual(Hit.ImpactNormal.Z, 1.f, 0.1f)) OnImpact(Hit);
}
