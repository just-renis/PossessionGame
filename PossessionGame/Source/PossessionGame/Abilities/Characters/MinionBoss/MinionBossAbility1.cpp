#include "MinionBossAbility1.h"
#include "PossessionGame/AI/Enemies/Animation Instances/MinionBossAnimationInstance.h"
#include "PossessionGame/AI/Enemies/Characters/MinionBoss.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

void UMinionBossAbility1::AI_Execute()
{
	Proprietor->SetIsActionInMotion(true);
	Proprietor->SetIsAbilityInAction(true);
	Proprietor->GetAnimInstance()->PlayMontage(EEnemyAnimState::Stomp);
	Proprietor->PlaySound(ESound::MinionBossStomp, true);
	Proprietor->GetWorld()->GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->SpawnVfx(EVfx::MinionBossStompCrack, [this]() -> bool { return Proprietor->IsAsyncAssetStillAllowedToBeUsed(); }, Proprietor->GetSocketLocation("StompSocket"));
	Cast<UMinionBossAnimationInstance>(Proprietor->GetAnimInstance())->SetIsActionMontagePlaying(true);
	Super::AI_Execute();
}

bool UMinionBossAbility1::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void UMinionBossAbility1::SetProprietor(const TObjectPtr<AEnemy>& InAbilityProprietor) { Proprietor = Cast<AMinionBoss>(InAbilityProprietor); }

void UMinionBossAbility1::CooldownLogic(const float InDeltaTime)
{
	Super::CooldownLogic(InDeltaTime);
	if (CurrentChargesAmount > 0) Proprietor->SetStompAvailability(true);
}
