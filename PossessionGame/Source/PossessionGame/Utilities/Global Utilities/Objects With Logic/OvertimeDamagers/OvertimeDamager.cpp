#include "OvertimeDamager.h"
#include "Components/SphereComponent.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Enums/EDamageReason.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"

void AOvertimeDamager::Setup(const float InDamage, const float InDamageTick, const float InLifeSpan, const float InRadiusSize, const EDamageReason InDamageReason)
{
	Damage = InDamage;
	DamageReason = InDamageReason;
	DamageTick = InDamageTick;
	Radius->SetSphereRadius(InRadiusSize);
	GetWorldTimerManager().SetTimer(DamageOvertimeHandle, this, &AOvertimeDamager::DealOvertimeDamage, DamageTick, true);
	GetWorldTimerManager().SetTimer(DestroyThisHandle, this, &AOvertimeDamager::DestroyThis, InLifeSpan, false);
}

AOvertimeDamager::AOvertimeDamager()
{
	Radius = CreateDefaultSubobject<USphereComponent>("radius");
	RootComponent = Radius;
	Radius->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Radius->OnComponentBeginOverlap.AddDynamic(this, &AOvertimeDamager::AddOverlappedTarget);
	Radius->OnComponentEndOverlap.AddDynamic(this, &AOvertimeDamager::RemoveOverlappedTarget);
}

void AOvertimeDamager::Init(const bool bInIsPlayerObject)
{
	Super::Init(bInIsPlayerObject);
	Radius->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	bIsPlayer = bInIsPlayerObject;
}

void AOvertimeDamager::AddTarget(const TObjectPtr<AEnemy>& InTarget) { Targets.Add(InTarget); }

void AOvertimeDamager::RemoveTarget(const TObjectPtr<AEnemy>& InTarget) { Targets.Remove(InTarget); }

void AOvertimeDamager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(DamageOvertimeHandle);
	GetWorldTimerManager().ClearTimer(DestroyThisHandle);
	Super::EndPlay(EndPlayReason);
}

void AOvertimeDamager::DestroyThis()
{
	GetWorldTimerManager().ClearTimer(DamageOvertimeHandle);
	GetWorld()->GetSubsystem<UPoolingManager>()->ReturnObjectToPool(EPoolObject::PoisonArea, this);
}

void AOvertimeDamager::AddOverlappedTarget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (const TObjectPtr<AEnemy> Target = Cast<AEnemy>(OtherActor)) AddTarget(Target);
}

void AOvertimeDamager::RemoveOverlappedTarget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (const TObjectPtr<AEnemy> Target = Cast<AEnemy>(OtherActor)) RemoveTarget(Target);
}