#include "PoisonGrenade.h"
#include "Components/AudioComponent.h"
#include "PossessionGame/Enums/EDamageReason.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Enums/EUpgradeAttribute.h"
#include "PossessionGame/Utilities/Data Utilities/FNiagaraAttribute.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/OvertimeDamagers/OvertimeDamager.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/OvertimeDamagers/PoisonArea.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"

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
	const float EffectDurationIncrease = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetUpgradeAttributeValues(EUpgrade::TricksterAbility1, EUpgradeAttribute::EffectDuration, ProjectileData.bIsPlayerProjectile);
	ModifiedOvertimeAreaLifeSpan = ProjectileData.OvertimeAreaLifeSpan;
	ModifiedOvertimeAreaSize = DefaultOvertimeAreaSize = ProjectileData.OvertimeAreaSize;
	ModifiedOvertimeAreaLifeSpan += ProjectileData.OvertimeAreaLifeSpan * EffectDurationIncrease * 0.01f;
	const float SizeIncrease = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetUpgradeAttributeValues(EUpgrade::TricksterAbility1, EUpgradeAttribute::Size, ProjectileData.bIsPlayerProjectile);
	ModifiedOvertimeAreaSize += DefaultOvertimeAreaSize * SizeIncrease * 0.01f;
	Attributes.Add(FName("Loop Duration"), FNiagaraAttribute(true, ModifiedOvertimeAreaLifeSpan));
	Attributes.Add(FName("Uniform Sprite Size Max"), FNiagaraAttribute(true, ModifiedOvertimeAreaSize));
	Attributes.Add(FName("Uniform Sprite Size Min"), FNiagaraAttribute(true, ModifiedOvertimeAreaSize));
	SpawnProjectileImpactVfx(Hit, false);
}

void APoisonGrenade::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
	const TObjectPtr<AOvertimeDamager> PoisonArea = GetWorld()->SpawnActor<AOvertimeDamager>(APoisonArea::StaticClass(), Hit.Location, Hit.Normal.Rotation());
	PoisonArea->Setup(ProjectileData.bIsPlayerProjectile, Damage, ProjectileData.OvertimeTickTrigger, ModifiedOvertimeAreaLifeSpan, ModifiedOvertimeAreaSize, EDamageReason::TricksterPoisonDamage);
}

void APoisonGrenade::SpawnProjectileImpactVfx(const FHitResult& InHit, const bool bFirstImpact)
{
	if (ProjectileData.bIsPlayerProjectile) AssetsManager->SpawnVfx(bFirstImpact ? EVfx::TricksterPoison : EVfx::TricksterPoisonImpact, nullptr, InHit.ImpactPoint, InHit.ImpactNormal.Rotation(), ProjectileData.GetProprietor()->GetVariation(), Attributes);
	else AssetsManager->SpawnVfx(bFirstImpact ? EVfx::AI_TricksterPoison : EVfx::AI_TricksterPoisonImpact, nullptr, InHit.ImpactPoint, InHit.ImpactNormal.Rotation(), ProjectileData.GetProprietor()->GetVariation(), Attributes);
}
