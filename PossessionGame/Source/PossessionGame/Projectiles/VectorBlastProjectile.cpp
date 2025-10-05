#include "VectorBlastProjectile.h"
#include "Components/AudioComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/EnemyCommunicator.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"

AVectorBlastProjectile::AVectorBlastProjectile()
{
	HitSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("HitSfx"));
	HitSfx->SetAutoActivate(false);
	HitSfx->SetupAttachment(Mesh);
	BounceSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("BounceSfx"));
	BounceSfx->SetAutoActivate(false);
	BounceSfx->SetupAttachment(Mesh);
	Sounds.Add(ESound::VectorBlastHit, HitSfx);
	Sounds.Add(ESound::VectorBlastBounce, BounceSfx);
}

float AVectorBlastProjectile::GetModifiedLifeSpan() const
{
	return Super::GetModifiedLifeSpan() + Super::GetModifiedLifeSpan() * GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetUpgradeAttributeValues(EUpgrade::GuardianAbility3, EUpgradeAttribute::EffectDuration, true) * 0.01f;
}

float AVectorBlastProjectile::GetModifiedProjectileInitialSpeed(const float InBaseInitialSpeed) const
{
	return InBaseInitialSpeed * GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetUpgradeAttributeValues(EUpgrade::GuardianAbility3, EUpgradeAttribute::ProjectileSpeed, true) * 0.01f;
}

float AVectorBlastProjectile::GetModifiedProjectileMaxSpeed(const float InBaseMaxSpeed) const
{
	return InBaseMaxSpeed * GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetUpgradeAttributeValues(EUpgrade::GuardianAbility3, EUpgradeAttribute::ProjectileSpeed, true) * 0.01f;
}

void AVectorBlastProjectile::Init(FProjectileData& InProjectileData, const FRotator& Rotation, const int32 InWhich)
{
	CustomTimeDilation = InProjectileData.GetProprietor()->CustomTimeDilation;
	MovementComponent->SetComponentTickInterval(1.f / 30.f / CustomTimeDilation);
	Super::Init(InProjectileData, Rotation, InWhich);
}

void AVectorBlastProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor == this || OtherActor == nullptr) return;
	SpawnFirstProjectileImpactVfx(Hit);
	if (OtherActor->ActorHasTag("Destructible"))
	{
		PlaySound(ESound::VectorBlastHit);
		UGameplayStatics::ApplyPointDamage(OtherActor, Damage, (Hit.Location - GetActorLocation()).GetSafeNormal() * 2.f, Hit, ProjectileData.GetProprietor() ? ProjectileData.GetProprietor()->GetController() : nullptr, this, UDamageType::StaticClass());
		return;
	}
	if (!OtherActor->ActorHasTag("Enemy"))
	{
		PlaySound(ESound::VectorBlastBounce);
		return;
	}
	if (Mesh->GetCollisionObjectType() == ECC_GameTraceChannel1)
	{
		PlaySound(ESound::VectorBlastHit);
		DealDamageToSentientActor(true, OtherActor, Hit);
		if (!GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->IsSpecialUpgradeAvailable(EUpgrade::GuardianAbility3, EUpgradeAttribute::Special1)) return;
		Cast<AEnemy>(OtherActor)->AI_TriggerVectorBlastSpecialImpact();
		if (const TObjectPtr<AActor> ClosestEnemy = GetWorld()->GetSubsystem<UEnemyCommunicator>()->GetClosestEnemy(GetActorLocation(), OtherActor))
		{
			UGameplayStatics::ApplyDamage(ClosestEnemy, Damage, ProjectileData.GetProprietor()->GetController(), this, UDamageType::StaticClass());
		}
	}
}
