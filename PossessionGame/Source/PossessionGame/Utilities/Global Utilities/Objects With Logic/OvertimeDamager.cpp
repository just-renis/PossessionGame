#include "OvertimeDamager.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Enums/EDamageReason.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"

void AOvertimeDamager::Setup(const float InDamage, const float InDamageTick, const float InLifeSpan, const float InRadiusSize, const EDamageReason InDamageReason)
{
	Damage = InDamage;
	DamageReason = InDamageReason;
	Radius->SetSphereRadius(InRadiusSize);
	UKismetSystemLibrary::K2_SetTimer(this, "DealOvertimeDamage", InDamageTick, true);
	UKismetSystemLibrary::K2_SetTimer(this, "DestroyThis", InLifeSpan, false);
}

AOvertimeDamager::AOvertimeDamager()
{
	Radius = CreateDefaultSubobject<USphereComponent>("radius");
	RootComponent = Radius;
	Radius->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Radius->OnComponentBeginOverlap.AddDynamic(this, &AOvertimeDamager::AddOverlappedTarget);
	Radius->OnComponentEndOverlap.AddDynamic(this, &AOvertimeDamager::RemoveOverlappedTarget);
}

void AOvertimeDamager::Init(bool bInIsPlayerObject)
{
	Super::Init(bInIsPlayerObject);
	bIsPlayer = bInIsPlayerObject;
}

void AOvertimeDamager::DestroyThis()
{
	UKismetSystemLibrary::K2_ClearTimer(this, "DealOvertimeDamage");
	GetWorld()->GetSubsystem<UPoolingManager>()->ReturnObjectToPool(EPoolObject::PoisonArea, this);
}

void AOvertimeDamager::DealOvertimeDamage() { for (int32 i = 0; i < Targets.Num(); i++) if (Targets[i]) Targets[i]->ReceiveDamage(Damage, DamageReason); }

void AOvertimeDamager::AddOverlappedTarget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (const TObjectPtr<AEnemy> Target = Cast<AEnemy>(OtherActor)) Targets.Add(Target);
}

void AOvertimeDamager::RemoveOverlappedTarget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (const TObjectPtr<AEnemy> Target = Cast<AEnemy>(OtherActor)) Targets.Remove(Target);
}