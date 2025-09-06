#include "Destructible.h"
#include "Engine/DamageEvents.h"
#include "PossessionGame/Data Assets/Base Data Assets/DestructibleData.h"
#include "PossessionGame/Projectiles/Projectile.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
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
	SetThisLifeSpan();
}

void ADestructible::Init(const bool bInIsPlayerObject)
{
	Super::Init(bInIsPlayerObject);
	Hp = Data->GetHp();
	bIsAffectedByPhysics = Data->GetIsAffectedByPhysics();
	bIsPlayerObject = bInIsPlayerObject;
	Mesh->SetCollisionProfileName(bIsPlayerObject ? "PlayerDestructible" : "EnemyDestructible");
	if (Data->GetIsLifeSpanImmediatelyAdded()) SetThisLifeSpan();
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

void ADestructible::SetThisLifeSpan()
{
	GetWorldTimerManager().SetTimer(DestroyHandle, this, &ADestructible::DestroyThis, LifeSpan, false);
}

float ADestructible::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser) * DamageReduction;
	if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
		const bool bIsPlayerProjectile = Cast<AProjectile>(DamageCauser)->GetIsPlayerProjectile();
		if ((bIsPlayerProjectile && !bIsPlayerObject) || (!bIsPlayerProjectile && bIsPlayerObject)) Hp -= Damage;
	}
	else Hp -= Damage;
	if (!bIsPlayerObject) (void)GetGameInstance()->GetSubsystem<UDelegateManager>()->ShowDamageNumber.ExecuteIfBound(FLinearColor::White, Damage, GetActorLocation(), GetActorForwardVector());
	if (Hp > 0) return Damage;
	if (bIsAffectedByPhysics)
	{
		HandleDeactivation();
		if (!DamageEvent.IsOfType(FRadialDamageEvent::ClassID)) return Damage;
		const FRadialDamageEvent* RadialDamageEvent = static_cast<const FRadialDamageEvent*>(&DamageEvent);
		Mesh->AddRadialImpulse(RadialDamageEvent->Origin, RadialDamageEvent->Params.OuterRadius, 20000.f, RIF_Linear);
	}
	else DestroyThis();
	return Damage;
}
