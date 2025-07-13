#include "PoisonGrenade.h"
#include "Components/AudioComponent.h"
#include "PossessionGame/Enums/EDamageReason.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/Enums/EUpgradeAttribute.h"
#include "PossessionGame/Enums/EVariation.h"
#include "PossessionGame/Utilities/Data Utilities/FNiagaraAttribute.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/OvertimeDamager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/UpgradeManager.h"
#include "PossessionGame/Utilities/Subsystems/VariationManager.h"

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
	ModifiedOvertimeAreaSize = DefaultOvertimeAreaSize = ProjectileData.OvertimeAreaSize;
	VariationAreaAppliance();
	ModifiedOvertimeAreaLifeSpan += ProjectileData.OvertimeAreaLifeSpan * EffectDurationIncrease * 0.01f;
	const float SizeIncrease = GetWorld()->GetSubsystem<UUpgradeManager>()->GetUpgradeAttributeValue(ProjectileData.bIsPlayerProjectile, EPoolObject::TricksterPoisonProjectile, EUpgradeAttribute::Size);
	ModifiedOvertimeAreaSize += DefaultOvertimeAreaSize * SizeIncrease * 0.01f;
	Attributes.Add("Loop Duration", FNiagaraAttribute(true, ModifiedOvertimeAreaLifeSpan));
	Attributes.Add("Uniform Sprite Size Max", FNiagaraAttribute(false, ModifiedOvertimeAreaSize));
	Attributes.Add("Uniform Sprite Size Min", FNiagaraAttribute(false, ModifiedOvertimeAreaSize));
	SpawnProjectileImpactVfx(Hit, false);
}

void APoisonGrenade::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
	const TObjectPtr<AOvertimeDamager> PoisonArea = Cast<AOvertimeDamager>(GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(EPoolObject::PoisonArea, Hit.Location, Hit.Normal.Rotation()));
	PoisonArea->Init(ProjectileData.bIsPlayerProjectile);
	PoisonArea->Setup(Damage, ProjectileData.OvertimeTickTrigger, ModifiedOvertimeAreaLifeSpan, ModifiedOvertimeAreaSize, EDamageReason::TricksterPoisonDamage);
}

void APoisonGrenade::VariationAreaAppliance()
{
	if (EVariation::TricksterVariationPurple != GetWorld()->GetSubsystem<UVariationManager>()->GetCurrentVariation(ProjectileData.GetProprietor()->GetCharacterName())) return;
	ModifiedOvertimeAreaSize = DefaultOvertimeAreaSize *= 0.5f;
}

void APoisonGrenade::VariationDamageAppliance()
{
	if (EVariation::TricksterVariationPurple != GetWorld()->GetSubsystem<UVariationManager>()->GetCurrentVariation(ProjectileData.GetProprietor()->GetCharacterName())) return;
	Damage = DefaultDamage = DefaultDamage * 2.5f;
}
