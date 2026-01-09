#include "OvertimeDamager.h"
#include "Components/SphereComponent.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Enums/EDamageReason.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"

void AOvertimeDamager::Setup(const bool bInIsPlayerObject, const float InDamage, const float InDamageTick, const float InLifeSpan, const float InRadiusSize, const EDamageReason InDamageReason, const FRotator& InRotation)
{
	bIsPlayer = bInIsPlayerObject;
	Damage = InDamage;
	DamageReason = InDamageReason;
	DamageTick = InDamageTick;
	Radius->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Radius->SetSphereRadius(InRadiusSize);
	Radius->SetGenerateOverlapEvents(true);
	Radius->UpdateOverlaps();
	GetWorldTimerManager().SetTimer(DamageOvertimeHandle, this, &AOvertimeDamager::DealOvertimeDamage, DamageTick, true);
	GetWorldTimerManager().SetTimer(DestroyThisHandle, this, &AOvertimeDamager::ReturnToPool, InLifeSpan, false);
}

AOvertimeDamager::AOvertimeDamager()
{
	Radius = CreateDefaultSubobject<USphereComponent>("radius");
	RootComponent = Radius;
	Radius->SetCollisionProfileName("TriggerPlayer");
	Radius->SetGenerateOverlapEvents(false);
	Radius->OnComponentBeginOverlap.AddDynamic(this, &AOvertimeDamager::AddOverlappedTarget);
	Radius->OnComponentEndOverlap.AddDynamic(this, &AOvertimeDamager::RemoveOverlappedTarget);
}

void AOvertimeDamager::AddTarget(const TObjectPtr<AEnemy>& InTarget) { Targets.Add(InTarget); }

void AOvertimeDamager::RemoveTarget(const TObjectPtr<AEnemy>& InTarget) { Targets.Remove(InTarget); }

void AOvertimeDamager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(DamageOvertimeHandle);
	GetWorldTimerManager().ClearTimer(DestroyThisHandle);
	Super::EndPlay(EndPlayReason);
}

void AOvertimeDamager::Disable(const bool bHide)
{
	Radius->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Targets.Empty();
	GetWorldTimerManager().ClearTimer(DamageOvertimeHandle);
	Super::Disable(bHide);
}

void AOvertimeDamager::ReturnToPool() { GetWorld()->GetSubsystem<UPoolingManager>()->ReturnObjectToPool(ObjectName, this); }

void AOvertimeDamager::AddOverlappedTarget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (const TObjectPtr<AEnemy> Target = Cast<AEnemy>(OtherActor))
	{
		if (!bIsPlayer && Target->GetIsHijacked()) AddTarget(Target);
		else if (bIsPlayer && !Target->GetIsHijacked()) AddTarget(Target);
	}
}

void AOvertimeDamager::RemoveOverlappedTarget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (const TObjectPtr<AEnemy> Target = Cast<AEnemy>(OtherActor))
	{
		if (!bIsPlayer && Target->GetIsHijacked()) RemoveTarget(Target);
		else if (bIsPlayer && !Target->GetIsHijacked()) RemoveTarget(Target);
	}
}