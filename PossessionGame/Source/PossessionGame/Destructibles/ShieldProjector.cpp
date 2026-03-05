#include "ShieldProjector.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "PossessionGame/Data Assets/Base Data Assets/DestructibleData.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

AShieldProjector::AShieldProjector()
{
	ProjectorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectorMesh"));
	RootComponent = ProjectorMesh;
	Mesh->SetupAttachment(RootComponent);
	DeploySfx = CreateDefaultSubobject<UAudioComponent>(TEXT("DeploySfx"));
	DeploySfx->SetAutoActivate(false);
	DeploySfx->SetupAttachment(ProjectorMesh);
	DissolveSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("DissolveSfx"));
	DissolveSfx->SetAutoActivate(false);
	DissolveSfx->SetupAttachment(ProjectorMesh);
	Sounds.Add(ESound::TankShieldDeploy, DeploySfx);
	Sounds.Add(ESound::TankShieldDissolve, DissolveSfx);
}

void AShieldProjector::BeginPlay()
{
	Super::BeginPlay();
	ProjectorMesh->OnComponentHit.AddDynamic(this, &AShieldProjector::OnProjectorLanded);
}

void AShieldProjector::Init(const bool bInIsPlayerObject)
{
	Super::Init(bInIsPlayerObject);
	if (!ConnectionVfx) GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->AttachVfx(EVfx::TankShieldConnection,[this]() -> bool { return ProjectorMesh->GetVisibleFlag(); }, &ConnectionVfx, ProjectorMesh);
	else ConnectionVfx->Activate();
	ChangeProjectorCollisionState(true);
}

void AShieldProjector::DestroyThis()
{
	ChangeProjectorCollisionState(false);
	Super::DestroyThis();
}

void AShieldProjector::ChangeProjectorCollisionState(const bool bState) const
{
	ProjectorMesh->SetSimulatePhysics(bState);
	ProjectorMesh->SetCollisionEnabled(bState ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
	ProjectorMesh->SetNotifyRigidBodyCollision(bState);
	if (!Mesh) return;
	Mesh->SetVisibility(!bState);
	Mesh->SetCollisionEnabled(bState ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryOnly);
}

void AShieldProjector::SetThisLifeSpan()
{
	LifeSpan = Data->GetLifeSpan();
	Super::SetThisLifeSpan();
}

void AShieldProjector::SetUpgradesApplication()
{
	DamageReduction = Data->GetDamageReduction();
}

void AShieldProjector::Disable(const bool bHide)
{
	PlaySound(ESound::TankShieldDissolve);
	Super::Disable(bHide);
	if (ConnectionVfx) ConnectionVfx->DeactivateImmediate();
}

void AShieldProjector::PlaySound(const ESound InSoundName)
{
	Sounds[InSoundName]->Sound ? Sounds[InSoundName]->Play() : GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->ConfigureSound(Sounds[InSoundName], TFunction<bool()>(), InSoundName);
}

void AShieldProjector::OnProjectorLanded(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!ProjectorMesh) return;
	PlaySound(ESound::TankShieldDeploy);
	ChangeProjectorCollisionState(false);
}
