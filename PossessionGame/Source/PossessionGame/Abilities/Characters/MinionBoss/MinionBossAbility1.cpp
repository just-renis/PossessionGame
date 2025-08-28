#include "MinionBossAbility1.h"
#include "Engine/OverlapResult.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/AI/Enemies/Animation Instances/MinionBossAnimationInstance.h"
#include "PossessionGame/AI/Enemies/Characters/MinionBoss.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

void UMinionBossAbility1::AI_Execute()
{
	Proprietor->SetIsActionInMotion(true);
	Proprietor->SetIsAbilityInAction(true);
	Proprietor->GetAnimInstance()->PlayMontage(EEnemyAnimState::Stomp);
	Proprietor->PlaySound(ESound::MinionBossStomp, true);
	Cast<UMinionBossAnimationInstance>(Proprietor->GetAnimInstance())->SetIsActionMontagePlaying(true);
	Proprietor->GetWorld()->GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->SpawnVfx(EVfx::MinionBossStompCrack, [this]() -> bool { return Proprietor->IsAsyncAssetStillAllowedToBeUsed(); }, Proprietor->GetSocketLocation("StompSocket"));
	GetWorld()->GetTimerManager().SetTimer(StompHandle, this, &UMinionBossAbility1::StompHitTheGround, 1.22f, false);
	Super::AI_Execute();
}

void UMinionBossAbility1::AI_StopAbility()
{
	Proprietor->StopSound(ESound::MinionBossStomp);
	GetWorld()->GetTimerManager().ClearTimer(StompHandle);
	Super::AI_StopAbility();
}

bool UMinionBossAbility1::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void UMinionBossAbility1::SetProprietor(const TObjectPtr<AEnemy> AbilityProprietor) { Proprietor = Cast<AMinionBoss>(AbilityProprietor); }

void UMinionBossAbility1::CooldownLogic(const float InDeltaTime)
{
	Super::CooldownLogic(InDeltaTime);
	if (CurrentChargesAmount > 0)
	{
		if (!Proprietor->GetIsAbilityInAction()) Proprietor->ActivateStompTrigger();
		else Proprietor->SetIsStompReady(true);
	}
}

void UMinionBossAbility1::StompHitTheGround() const
{
	Proprietor->TriggerStompCameraShake();
	const FVector StompLocation = Proprietor->GetSocketLocation("StompSocket");
	TArray<FOverlapResult> Overlaps;
	Proprietor->GetWorld()->OverlapMultiByProfile(Overlaps, StompLocation, FQuat::Identity, FName("TriggerPlayer"), FCollisionShape::MakeSphere(Proprietor->GetStompRadius()));
	TSet<TObjectPtr<AActor>> UniqueActors;
	for (const FOverlapResult& Result : Overlaps)
	{
		TObjectPtr<AActor> HitActor = Result.GetActor();
		if (!HitActor || UniqueActors.Contains(HitActor)) continue;
		UniqueActors.Add(HitActor);
		if (HitActor->ActorHasTag("Player")) UGameplayStatics::ApplyDamage(HitActor, Proprietor->GetStompDamage(), nullptr, Proprietor, UDamageType::StaticClass());
	}
}
