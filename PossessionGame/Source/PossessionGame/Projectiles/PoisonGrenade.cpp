#include "PoisonGrenade.h"
#include "Components/AudioComponent.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/Enums/EUpgradeAttribute.h"
#include "PossessionGame/Utilities/Data Utilities/FNiagaraAttribute.h"
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

void APoisonGrenade::BeginPlay()
{
	Super::BeginPlay();
	Attributes.Add(TEXT("User.Loop Duration"), FNiagaraAttribute(true, 0.f));
	Attributes.Add(TEXT("User.Uniform Sprite Size Max"), FNiagaraAttribute(false, 0.f));
	Attributes.Add(TEXT("User.Uniform Sprite Size Min"), FNiagaraAttribute(false, 0.f));
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
	ModifiedOvertimeAreaSize = ProjectileData.OvertimeAreaSize;
	if (EffectDurationIncrease > 0.f) ModifiedOvertimeAreaLifeSpan += ProjectileData.OvertimeAreaLifeSpan * (EffectDurationIncrease / 100.f);
	const float SizeIncrease = GetWorld()->GetSubsystem<UUpgradeManager>()->GetUpgradeAttributeValue(ProjectileData.bIsPlayerProjectile, EPoolObject::TricksterPoisonProjectile, EUpgradeAttribute::Size);
	if (SizeIncrease > 0.f) ModifiedOvertimeAreaSize += ProjectileData.OvertimeAreaSize * (SizeIncrease / 100.f);
	Attributes["User.Loop Duration"].Value = ModifiedOvertimeAreaLifeSpan;
	Attributes["User.Uniform Sprite Size Max"].Value = SizeIncrease;
	Attributes["User.Uniform Sprite Size Min"].Value = SizeIncrease;
	SpawnProjectileImpactVfx(Hit, false, Attributes);
}

void APoisonGrenade::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
	const TObjectPtr<AOvertimeDamager> PoisonArea = Cast<AOvertimeDamager>(GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(EPoolObject::PoisonArea, Hit.Location, Hit.Normal.Rotation()));
	PoisonArea->Init(ProjectileData.bIsPlayerProjectile);
	PoisonArea->Setup(Damage, ProjectileData.OvertimeTickTrigger, ModifiedOvertimeAreaLifeSpan, ModifiedOvertimeAreaSize);
}
