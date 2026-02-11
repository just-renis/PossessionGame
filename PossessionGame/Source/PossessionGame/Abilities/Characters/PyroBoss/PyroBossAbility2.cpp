#include "PyroBossAbility2.h"
#include "Engine/OverlapResult.h"
#include "PossessionGame/AI/Enemies/Characters/PyroBoss.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"

void UPyroBossAbility2::AI_Execute()
{
	Proprietor->SetIsAbilityInAction(true);
	Proprietor->GetAnimInstance()->SetNonBlendableSlotState(true);
	Proprietor->GetAnimInstance()->PlayMontage(EEnemyAnimState::Stomp);
	//Proprietor->PlaySound(ESound::MinionBossStomp, true);
	//Proprietor->GetWorld()->GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->SpawnVfx(EVfx::MinionBossStompCrack, [this]() -> bool { return Proprietor->IsAsyncAssetStillAllowedToBeUsed(); }, Proprietor->GetSocketLocation("StompSocket"));
	GetWorld()->GetTimerManager().SetTimer(FirstStompHandle, this, &UPyroBossAbility2::StompHitTheGround, 0.88f, false);
	GetWorld()->GetTimerManager().SetTimer(SecondStompHandle, this, &UPyroBossAbility2::StompHitTheGround, 1.59f, false);
	Super::AI_Execute();
}

void UPyroBossAbility2::AI_StopAbility()
{
	Proprietor->GetWorldTimerManager().ClearTimer(FirstStompHandle);
	Proprietor->GetWorldTimerManager().ClearTimer(SecondStompHandle);
	Super::AI_StopAbility();
}

bool UPyroBossAbility2::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void UPyroBossAbility2::SetProprietor(const TObjectPtr<AEnemy>& InAbilityProprietor) { Proprietor = Cast<APyroBoss>(InAbilityProprietor); }

void UPyroBossAbility2::StompHitTheGround()
{
//	Proprietor->TriggerStompCameraShake();
	const FVector StompLocation = Proprietor->GetSocketLocation("StompSocket");
	TArray<FOverlapResult> Overlaps;
	Proprietor->GetWorld()->OverlapMultiByProfile(Overlaps, StompLocation, FQuat::Identity, FName("TriggerPlayer"), FCollisionShape::MakeSphere(Proprietor->GetStompRadius() * 100.f));
	TSet<TObjectPtr<AActor>> UniqueActors;
	for (const FOverlapResult& Result : Overlaps)
	{
		TObjectPtr<AActor> HitActor = Result.GetActor();
		if (!HitActor || UniqueActors.Contains(HitActor)) continue;
		UniqueActors.Add(HitActor);
		if (!HitActor->ActorHasTag("Player")) return;
		Cast<AEnemy>(HitActor)->P_ReceiveStunEffect(bFirstStompSucceed ? 0.f : Proprietor->GetStunDuration(), Proprietor->GetStompDamage(), FVector::ZeroVector);
		bFirstStompSucceed = true;
	}
}

void UPyroBossAbility2::CooldownLogic(const float InDeltaTime)
{
	Super::CooldownLogic(InDeltaTime);
	if (CurrentChargesAmount > 0)
	{
		bFirstStompSucceed = false;
		Proprietor->MarkStompAbilityReady();
	}
}
