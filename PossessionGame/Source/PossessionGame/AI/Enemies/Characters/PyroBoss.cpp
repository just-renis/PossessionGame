#include "PyroBoss.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EDamageReason.h"
#include "PossessionGame/Enums/EEnemyHpBar.h"
#include "PossessionGame/Enums/EWeapon.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"

void APyroBoss::AI_ReceiveStunEffect(const bool bHitByOtherAI, const float InEffectDuration, const float InDamage, FVector InHitDirection)
{
	if (bHitByOtherAI) return;
	(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->ShowDamageNumber.ExecuteIfBound(GetDamageNumberColor(), InDamage, GetActorLocation(), GetActorForwardVector());
	ReceiveDamage(InDamage, EDamageReason::BreacherBashDamage, nullptr, nullptr);
}

void APyroBoss::AI_SetupWeapon()
{
	Weapon = GetWorld()->GetGameInstance()->GetSubsystem<UWeaponManager>()->GetWeapon(GetWorld(), EWeapon::PyroBossWeapon, GetMesh());
	Super::AI_SetupWeapon();
}

void APyroBoss::BeginPlay()
{
	CharacterName = ECharacterType::PyroBoss;
	Super::BeginPlay();
	(void)DelegateManager->OnGlobalHpBarSpawn.ExecuteIfBound(EEnemyHpBar::MinionBoss);
}

void APyroBoss::InitiateDeath()
{
	Super::InitiateDeath();
	DelegateManager->OnBossDeath.Broadcast();
}

void APyroBoss::ReceiveDamage(float InDamage, const EDamageReason InDamageReason, const FPointDamageEvent* PointDamageEvent, const FRadialDamageEvent* RadialDamageEvent)
{
	Super::ReceiveDamage(InDamage, InDamageReason, PointDamageEvent, RadialDamageEvent);
	(void)DelegateManager->UpdateGlobalHpBar.ExecuteIfBound(CurrentHp, MaxHp);
}

void APyroBoss::ApplyEmpEffect(const float InEffectDuration) { Super::ApplyEmpEffect(InEffectDuration * 0.5f); }
