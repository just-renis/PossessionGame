#include "MinionBossAbility2.h"
#include "NiagaraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/AI/Enemies/Characters/MinionBoss.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "PossessionGame/Enums/ESound.h"

void UMinionBossAbility2::AI_Execute()
{
	Proprietor->SetActionInMotion(true);
	Proprietor->GetAnimInstance()->PlayMontage(EEnemyAnimState::PrepareLaser);
	Proprietor->PlaySound(ESound::MinionBossLaserEyeCharge);
	Proprietor->EnableLaserEyeBall();
	UKismetSystemLibrary::K2_SetTimer(this, "ShootLaser", 1.65f, false);
}

bool UMinionBossAbility2::IsProprietorPossessed() { return Proprietor->GetIsPossessed(); }

void UMinionBossAbility2::SetProprietor(const TObjectPtr<AEnemy> AbilityProprietor) { Proprietor = Cast<AMinionBoss>(AbilityProprietor); }

void UMinionBossAbility2::CooldownLogic(const float InDeltaTime)
{
	Super::CooldownLogic(InDeltaTime);
	if (CurrentCooldownSecs >= ReqCooldownSecs) Proprietor->EnableDistanceCheckForLaserEye();
}

void UMinionBossAbility2::StopLaser()
{
	UKismetSystemLibrary::K2_ClearTimer(this, "UpdateLaserEndLocation");
	LaserTime = 0.f;
	Proprietor->GetLaser()->Deactivate();
	Proprietor->GetLaserEyeBall()->Deactivate();
	Proprietor->GetAnimInstance()->PlayMontage(EEnemyAnimState::StopLaser);
	ResetCooldown();
}

void UMinionBossAbility2::ShootLaser()
{
	Proprietor->EnableLaser();
	UKismetSystemLibrary::K2_SetTimer(this, "UpdateLaserEndLocation", 0.025f, true);
}

void UMinionBossAbility2::UpdateLaserEndLocation()
{
	if (!Proprietor || !Proprietor->GetLaser()) return;
	const TObjectPtr<AActor> Player = Proprietor->GetAI()->GetStateTreeData().GetTarget();
	if (!Player) return;
	const FVector Start = Proprietor->GetMesh()->GetSocketTransform(TEXT("LaserSocket"), RTS_World).GetLocation();
	FVector Direction = Player->GetActorLocation() - Start;
	const float Distance = Direction.Size();
	Direction /= Distance;
	const FRotator DesiredRot = Direction.Rotation();
	const FRotator CurrentRot = Proprietor->GetLaser()->GetComponentRotation();
	const FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(DesiredRot, CurrentRot);
	FRotator NewRot = CurrentRot;
	NewRot.Yaw += FMath::Clamp(DeltaRot.Yaw, -0.75f, 0.75f);
	NewRot.Pitch += FMath::Clamp(DeltaRot.Pitch, -0.75f, 0.75f);
	NewRot.Roll = 0.f;
	Proprietor->GetLaser()->SetWorldLocationAndRotation(Start, NewRot);
	Proprietor->GetLaser()->SetFloatParameter(TEXT("_Length"), Distance * 0.001f);
	LaserTime += 0.025f;
	if (LaserTime < 3.f) return;
	StopLaser();
}
