#include "OvertimeDamager.h"
#include "Components/SphereComponent.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Data Assets/Base Data Assets/DamageOverTimeData.h"
#include "PossessionGame/Enums/EDamageReason.h"
#include "PossessionGame/Enums/EVariation.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/SaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"

void AOvertimeDamager::Setup(const bool bInIsPlayerObject, const TObjectPtr<UDamageOverTimeData>& InDamageOverTimeData, const EDamageReason InDamageReason)
{
	bIsActive = true;
	bIsPlayer = bInIsPlayerObject;
	Damage = DefaultDamage = InDamageOverTimeData->GetDamage();
	DamageBoostPercentage = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetPlayerLevelAllGlobalDamageBonus() + GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetSecretBoost(EVariation::SecretCrateRewardDamage) * 0.01f;
	DamageReason = InDamageReason;
	DamageTick = DefaultDamageTick = InDamageOverTimeData->GetDamageTick();
	EffectTick = InDamageOverTimeData->GetEffectDuration();
	SetupEffectStrength(InDamageOverTimeData);
}

void AOvertimeDamager::Setup(const bool bInIsPlayerObject, const float InDamage, const float InDamageTick, const float InLifeSpan, const float InRadiusSize, const EDamageReason InDamageReason)
{
	bIsActive = true;
	bIsPlayer = bInIsPlayerObject;
	Damage = DefaultDamage = InDamage;
	DamageBoostPercentage = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetPlayerLevelAllGlobalDamageBonus() + GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetSecretBoost(EVariation::SecretCrateRewardDamage) * 0.01f;
	DamageReason = InDamageReason;
	DamageTick = DefaultDamageTick = InDamageTick;
	SetupRadiusAndTimers(InRadiusSize, InLifeSpan);
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

void AOvertimeDamager::SetupRadiusAndTimers(const float InAreaSize, const float InLifeSpan)
{
	Radius->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Radius->SetSphereRadius(InAreaSize);
	Radius->SetGenerateOverlapEvents(true);
	Radius->UpdateOverlaps();
	GetWorldTimerManager().SetTimer(DamageOvertimeHandle, this, &AOvertimeDamager::DealOvertimeDamage, DamageTick, true);
	GetWorldTimerManager().SetTimer(DestroyThisHandle, this, &AOvertimeDamager::ReturnToPool, InLifeSpan, false);
}

void AOvertimeDamager::Disable(const bool bHide)
{
	bIsActive = false;
	Radius->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Targets.Empty();
	GetWorldTimerManager().ClearTimer(DamageOvertimeHandle);
	Super::Disable(bHide);
}

void AOvertimeDamager::SetupEffectStrength(const TObjectPtr<UDamageOverTimeData>& InDamageOverTimeData) { EffectStrength = InDamageOverTimeData->GetEffectStrength(); }

void AOvertimeDamager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(DamageOvertimeHandle);
	GetWorldTimerManager().ClearTimer(DestroyThisHandle);
	Super::EndPlay(EndPlayReason);
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