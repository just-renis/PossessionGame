#include "OvertimeDamager.h"
#include "Components/SphereComponent.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Enums/EDamageReason.h"

void AOvertimeDamager::Setup(const bool bInIsPlayerObject, const float InDamage, const float InDamageTick, const float InLifeSpan, const float InRadiusSize, const EDamageReason InDamageReason)
{
	bIsPlayer = bInIsPlayerObject;
	Damage = InDamage;
	DamageReason = InDamageReason;
	DamageTick = InDamageTick;
	Radius->SetSphereRadius(InRadiusSize);
	GetWorldTimerManager().SetTimer(DamageOvertimeHandle, this, &AOvertimeDamager::DealOvertimeDamage, DamageTick, true);
	GetWorldTimerManager().SetTimer(DestroyThisHandle, this, &AOvertimeDamager::DestroyThis, InLifeSpan, false);
	Radius->SetGenerateOverlapEvents(true);
	Radius->UpdateOverlaps();
}

AOvertimeDamager::AOvertimeDamager()
{
	Radius = CreateDefaultSubobject<USphereComponent>("radius");
	RootComponent = Radius;
	Radius->SetCollisionProfileName("TriggerPlayer");
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

void AOvertimeDamager::DestroyThis()
{
	GetWorldTimerManager().ClearTimer(DamageOvertimeHandle);
	Destroy();
}

void AOvertimeDamager::AddOverlappedTarget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Error, TEXT("AddOverlappedTarget BITCH"));
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