#include "PyroBoss.h"

#include "NiagaraComponent.h"
#include "Algo/RandomShuffle.h"
#include "Engine/OverlapResult.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EDamageReason.h"
#include "PossessionGame/Enums/EEnemyHpBar.h"
#include "PossessionGame/Enums/EWeapon.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"

void APyroBoss::MarkFloorLavaAbilityReady()
{
	GetWorldTimerManager().SetTimer(ActivateFloorLavaHandle, this, &APyroBoss::ActivateFloorLava, 0.25f, true);
}

void APyroBoss::MarkStompAbilityReady()
{
	GetWorldTimerManager().SetTimer(ActivateStompHandle, this, &APyroBoss::ActivateStomp, 0.25f, true);
}

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

void APyroBoss::ActivateFloorLava()
{
	if (!AI || IsActionsDisabled() || bIsActionInMotion || !AI->GetStateTreeData().GetStateTreeRef()) return;
	SetIsActionInMotion(true);
	if (!SendSignalVfx) AssetsManager->AttachVfx(EVfx::PyroBossSendSignal, [this]() -> bool { return IsAsyncAssetStillAllowedToBeUsed(); }, &SendSignalVfx, GetMesh(), FName("DeathSocket"));
	else SendSignalVfx->Activate(true);
	GetWorldTimerManager().ClearTimer(ActivateFloorLavaHandle);
	AI->GetStateTreeData().TriggerEvent(EStateTreeEvent::PyroBossActivateFireFloor);
	const float HpPercentageLeft = CurrentHp / MaxHp;
	const int32 NumIndicesToUse = HpPercentageLeft > SecondPhaseHpPercentage ? 1.f : HpPercentageLeft > ThirdPhaseHpPercentage ? 2.f : 3.f;
	TArray Indices = { 0, 1, 2, 3, 4, 5, 6, 7 };
	Algo::RandomShuffle(Indices);
	Indices.SetNum(NumIndicesToUse);
	DelegateManager->TriggerFireFloors.Broadcast(Indices);
}

void APyroBoss::ActivateStomp()
{
	if (!AI || IsActionsDisabled() || bIsActionInMotion || !AI->GetStateTreeData().GetStateTreeRef()) return;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	TArray<FOverlapResult> Overlaps;
	GetWorld()->OverlapMultiByProfile(Overlaps, GetActorLocation(), FQuat::Identity, FName("TriggerPlayer"), FCollisionShape::MakeSphere(StompTriggerRadiusInMeters * 100.f), Params);
	for (const FOverlapResult& Overlap : Overlaps)
	{
		TObjectPtr<AActor> Actor = Overlap.GetActor();
		if (!Actor || !Actor->ActorHasTag("Player")) continue;
		SetIsActionInMotion(true);
		GetWorldTimerManager().ClearTimer(ActivateStompHandle);
		AI->GetStateTreeData().TriggerEvent(EStateTreeEvent::PyroBossStomp);
		return;
	}
}

void APyroBoss::ClearTimers()
{
	Super::ClearTimers();
	GetWorldTimerManager().ClearTimer(ActivateFloorLavaHandle);
	GetWorldTimerManager().ClearTimer(ActivateStompHandle);
}
