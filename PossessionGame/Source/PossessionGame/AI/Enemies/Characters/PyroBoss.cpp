#include "PyroBoss.h"

#include "Algo/RandomShuffle.h"
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
	GetWorldTimerManager().SetTimer(TESTHandle, this, &APyroBoss::TEST, 6.f, true);
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

void APyroBoss::TEST()
{
	TArray Indices = { 0, 1, 2, 3, 4, 5, 6, 7 };
	Algo::RandomShuffle(Indices);
	const int32 A = Indices[0];
	const int32 B = Indices[1];
	const int32 C = Indices[2];
	Indices = TArray { A, B, C };
	DelegateManager->TriggerFireFloors.Broadcast(Indices);
}
