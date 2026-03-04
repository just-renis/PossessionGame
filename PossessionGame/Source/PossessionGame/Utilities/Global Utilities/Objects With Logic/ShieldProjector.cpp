#include "ShieldProjector.h"
#include "PossessionGame/Data Assets/Base Data Assets/DestructibleData.h"

AShieldProjector::AShieldProjector()
{
	ProjectorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectorMesh"));
	RootComponent = ProjectorMesh;
	Mesh->SetupAttachment(RootComponent);
}

void AShieldProjector::BeginPlay()
{
	Super::BeginPlay();
	ProjectorMesh->OnComponentHit.AddDynamic(this, &AShieldProjector::OnProjectorLanded);
}

void AShieldProjector::Init(const bool bInIsPlayerObject)
{
	Super::Init(bInIsPlayerObject);
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

void AShieldProjector::OnProjectorLanded(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!ProjectorMesh) return;
	ChangeProjectorCollisionState(false);
}
