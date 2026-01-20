#include "WraithMolotov.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "PossessionGame/Data Assets/Base Data Assets/ThrowableData.h"
#include "PossessionGame/Enums/EDamageReason.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/OvertimeDamagers/OvertimeDamager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

AWraithMolotov::AWraithMolotov()
{
	DeploySfx = CreateDefaultSubobject<UAudioComponent>(TEXT("ImpactSfx"));
	DeploySfx->SetAutoActivate(false);
	DeploySfx->SetupAttachment(Mesh);
	Sounds.Add(ESound::WraithMolotovImpact, DeploySfx);
}

void AWraithMolotov::Init(const TObjectPtr<AEnemy> InProprietor)
{
	Super::Init(InProprietor);
	if (!TrailVfx) GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->AttachVfx(EVfx::WraithMolotovTrail, [this]() -> bool { return bIsActive; }, &TrailVfx, Mesh, FName("TrailSocket"));
	else TrailVfx->Activate(true);
}

void AWraithMolotov::BeginPlay()
{
	Super::BeginPlay();
	if (!Mesh->GetMaterial(0)) return;
	MI_MeshBaseMaterial = Mesh->CreateAndSetMaterialInstanceDynamic(0);
	MI_MeshBaseMaterial->SetVectorParameterValue("Color", FLinearColor(0.571f, 0.296f, 0.f));
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
