#include "Destructible.h"
#include "Engine/DamageEvents.h"
#include "PossessionGame/Data Assets/Base Data Assets/DestructibleData.h"
#include "PossessionGame/Projectiles/Projectile.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"

ADestructible::ADestructible()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("mesh"));
	RootComponent = Mesh;
}

void ADestructible::HandleDeactivation()
{
	bIsDeactivated = true;
	if (Mesh)
	{
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		Mesh->SetSimulatePhysics(true);
	}
	SetLifeSpan();
}

void ADestructible::Init(const bool bInIsPlayerObject)
{
	Super::Init(bInIsPlayerObject);
	Hp = Data->GetHp();
	bIsAffectedByPhysics = Data->GetIsAffectedByPhysics();
	bIsPlayerObject = bInIsPlayerObject;
	Mesh->SetCollisionProfileName(bIsPlayerObject ? "PlayerDestructible" : "EnemyDestructible");
	if (Data->GetIsLifeSpanImmediatelyAdded()) SetLifeSpan();
	SetUpgradesApplication();
}

void ADestructible::BeginPlay()
{
	Super::BeginPlay();
	Tags.Add("Destructible");
}

void ADestructible::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(DestroyHandle);
	Super::EndPlay(EndPlayReason);
}

void ADestructible::DestroyThis()
{
	GetWorldTimerManager().ClearTimer(DestroyHandle);
	GetWorld()->GetSubsystem<UPoolingManager>()->ReturnObjectToPool(Data->GetObjectName(), this);
}

void ADestructible::SetLifeSpan()
{
	GetWorldTimerManager().SetTimer(DestroyHandle, this, &ADestructible::DestroyThis, LifeSpan, false);
}

float ADestructible::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float Damage;
	if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
		Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
		const bool bIsPlayerProjectile = Cast<AProjectile>(DamageCauser)->GetIsPlayerProjectile();
		if ((bIsPlayerProjectile && !bIsPlayerObject) || (!bIsPlayerProjectile && bIsPlayerObject)) Hp -= Damage * DamageReduction;
	}
	else
	{
		Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
		Hp -= Damage * DamageReduction;
	}
	if (Hp <= 0)
	{
		if (bIsAffectedByPhysics)
		{
			HandleDeactivation();
			if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
			{
				const FRadialDamageEvent* RadialDamageEvent = static_cast<const FRadialDamageEvent*>(&DamageEvent);
				Mesh->AddRadialImpulse(RadialDamageEvent->Origin, RadialDamageEvent->Params.OuterRadius, 20000.f, RIF_Linear);
			}
		}
		else DestroyThis();
	}
	return Damage;
}
