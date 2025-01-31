#include "Destructible.h"
#include "Engine/DamageEvents.h"
#include "Kismet/KismetSystemLibrary.h"
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
	UKismetSystemLibrary::K2_SetTimer(this, "DestroyThis", LifeSpan, false);
}

void ADestructible::Init(bool bInIsPlayerObject)
{
	Super::Init(bInIsPlayerObject);
	LifeSpan = Data->GetLifeSpan();
	Hp = Data->GetHp();
	bIsAffectedByPhysics = Data->GetIsAffectedByPhysics();
	bIsPlayerObject = bInIsPlayerObject;
	Mesh->SetCollisionProfileName(bIsPlayerObject ? "PlayerDestructible" : "EnemyDestructible");
	if (Data->GetIsLifeSpanImmediatelyAdded()) UKismetSystemLibrary::K2_SetTimer(this, "DestroyThis", LifeSpan, false);
}

void ADestructible::BeginPlay()
{
	Super::BeginPlay();
	Tags.Add("Destructible");
}

void ADestructible::DestroyThis()
{
	GetWorld()->GetSubsystem<UPoolingManager>()->ReturnObjectToPool(Data->GetObjectName(), this);
	UKismetSystemLibrary::K2_ClearTimer(this, "DestroyThis");
}

float ADestructible::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float Damage;
	if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
		Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
		const bool bIsPlayerProjectile = Cast<AProjectile>(DamageCauser)->GetIsPlayerProjectile();
		if ((bIsPlayerProjectile && !bIsPlayerObject) || (!bIsPlayerProjectile && bIsPlayerObject)) Hp -= Damage;
	}
	else
	{
		Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
		Hp -= Damage;
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
