#include "WraithTrap.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "PossessionGame/Data Assets/Base Data Assets/ThrowableData.h"
#include "PossessionGame/Enums/EDamageReason.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/OvertimeDamagers/OvertimeDamager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

void AWraithTrap::InitWithProprietor(const TObjectPtr<AEnemy> InProprietor)
{
	Super::InitWithProprietor(InProprietor);
	if (!TrailVfx) GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->AttachVfx(EVfx::WraithTrapTrail, [this]() -> bool { return bIsActive; }, &TrailVfx, Mesh, FName("TrailSocket"));
	else TrailVfx->Activate(true);
}

AWraithTrap::AWraithTrap()
{
	DeploySfx = CreateDefaultSubobject<UAudioComponent>(TEXT("ImpactSfx"));
	DeploySfx->SetAutoActivate(false);
	DeploySfx->SetupAttachment(Mesh);
	Sounds.Add(ESound::WraithTrapImpact, DeploySfx);
}

void AWraithTrap::BeginPlay()
{
	Super::BeginPlay();
	if (!Mesh->GetMaterial(0)) return;
	MI_MeshBaseMaterial = Mesh->CreateAndSetMaterialInstanceDynamic(0);
	MI_MeshBaseMaterial->SetVectorParameterValue("Color", FLinearColor(0.3f, 0.f, 0.571f));
}

void AWraithTrap::OnImpact(const FHitResult& InHit)
{
	Super::OnImpact(InHit);
	PlaySound(ESound::WraithTrapImpact);
	const TObjectPtr<AOvertimeDamager> TrapArea = Cast<AOvertimeDamager>(GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(EPoolObject::WraithTrapArea, InHit.Location + FVector::UpVector, InHit.Normal.Rotation()));
	TrapArea->Init();
	TrapArea->Setup(true, Data->GetDamageOverTimeData(), EDamageReason::None);
	GetWorld()->GetSubsystem<UPoolingManager>()->ReturnObjectToPool(EPoolObject::WraithTrap, this);
}

void AWraithTrap::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (FMath::IsNearlyEqual(Hit.ImpactNormal.Z, 1.f, 0.1f)) OnImpact(Hit);
}
