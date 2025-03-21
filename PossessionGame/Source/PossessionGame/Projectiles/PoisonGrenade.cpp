#include "PoisonGrenade.h"
#include "Components/AudioComponent.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/Enums/EUpgradeAttribute.h"
#include "PossessionGame/Utilities/Global Utilities/OvertimeDamager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/UpgradeManager.h"

APoisonGrenade::APoisonGrenade()
{
	Sounds.Add(ESound::TricksterPoisonImpact, CreateDefaultSubobject<UAudioComponent>(TEXT("poisonImpactSound")));
	Sounds[ESound::TricksterPoisonImpact]->SetAutoActivate(false);
	Sounds[ESound::TricksterPoisonImpact]->SetupAttachment(Mesh);
	Sounds.Add(ESound::TricksterPoisonRelease, CreateDefaultSubobject<UAudioComponent>(TEXT("poisonReleaseSound")));
	Sounds[ESound::TricksterPoisonRelease]->SetAutoActivate(false);
	Sounds[ESound::TricksterPoisonRelease]->SetupAttachment(Mesh);
}

void APoisonGrenade::PlayImpactSound()
{
	PlaySound(ESound::TricksterPoisonImpact);
	PlaySound(ESound::TricksterPoisonRelease);
}

void APoisonGrenade::SpawnSecondProjectileImpactVfx(const FHitResult& Hit)
{
	const float EffectDurationIncrease = GetWorld()->GetSubsystem<UUpgradeManager>()->GetUpgradeAttributeValue(ProjectileData.bIsPlayerProjectile, EPoolObject::TricksterPoisonProjectile, EUpgradeAttribute::EffectDuration);
	ModifiedOvertimeAreaLifeSpan = ProjectileData.OvertimeAreaLifeSpan;
	if (EffectDurationIncrease > 0.f) ModifiedOvertimeAreaLifeSpan += ProjectileData.OvertimeAreaLifeSpan * (EffectDurationIncrease / 100.f);
	SpawnProjectileImpactVfx(Hit, false, TEXT("User.Loop Duration"), ModifiedOvertimeAreaLifeSpan);
}

void APoisonGrenade::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
	const TObjectPtr<AOvertimeDamager> PoisonArea = Cast<AOvertimeDamager>(GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(EPoolObject::PoisonArea, Hit.Location, Hit.Normal.Rotation()));
	PoisonArea->Init(ProjectileData.bIsPlayerProjectile);
	PoisonArea->Setup(Damage, ProjectileData.OvertimeTickTrigger, ProjectileData.OvertimeAreaLifeSpan, ProjectileData.OvertimeAreaSize);
}
