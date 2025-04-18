#include "MinionBossAbility1.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/AI/Enemies/Animation Instances/MinionBossAnimationInstance.h"
#include "PossessionGame/AI/Enemies/Characters/MinionBoss.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

void UMinionBossAbility1::AI_Execute()
{
	Proprietor->SetActionInMotion(true);
	Proprietor->GetAnimInstance()->PlayMontage(EEnemyAnimState::Stomp);
	Proprietor->PlaySound(ESound::MinionBossStomp);
	Cast<UMinionBossAnimationInstance>(Proprietor->GetAnimInstance())->SetStompIsPlaying(true);
	UKismetSystemLibrary::K2_SetTimer(this, "StompHitTheGround", 1.22f, false);
	Super::AI_Execute();
}

bool UMinionBossAbility1::IsProprietorPossessed() { return Proprietor->GetIsPossessed(); }

void UMinionBossAbility1::SetProprietor(const TObjectPtr<AEnemy> AbilityProprietor) { Proprietor = Cast<AMinionBoss>(AbilityProprietor); }

void UMinionBossAbility1::CooldownLogic(const float InDeltaTime)
{
	Super::CooldownLogic(InDeltaTime);
	if (CurrentCooldownSecs >= GetReqCooldownSecs()) Proprietor->AI_ToggleStompTrigger(true);
}

void UMinionBossAbility1::StompHitTheGround() const
{
	Proprietor->GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->SpawnOrLoadVfx(EVfx::MinionBossStompCrack, Proprietor->GetSocketLocation("StompSocket"));
}
