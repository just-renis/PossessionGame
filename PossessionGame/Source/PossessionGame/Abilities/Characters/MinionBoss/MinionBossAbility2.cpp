#include "MinionBossAbility2.h"
#include "NiagaraComponent.h"
#include "Components/CapsuleComponent.h"
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
	Proprietor->SetAbilityInAction(true);
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
	float Distance = Direction.Size();
	Direction /= Distance;
	const FRotator DesiredRot = Direction.Rotation();
	const FRotator CurrentRot = Proprietor->GetLaser()->GetComponentRotation();
	const FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(DesiredRot, CurrentRot);
	FRotator NewRot = CurrentRot;
	NewRot.Yaw += FMath::Clamp(DeltaRot.Yaw, -0.75f, 0.75f);
	NewRot.Pitch += FMath::Clamp(DeltaRot.Pitch, -0.75f, 0.75f);
	NewRot.Roll = 0.f;
	Proprietor->GetLaser()->SetWorldLocationAndRotation(Start, NewRot);
	
	const FVector LaserForward = NewRot.Vector();
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Proprietor);
	FHitResult LaserHit;
	const FVector LaserEnd = Start + LaserForward * 10000.f;
	GetWorld()->LineTraceSingleByChannel(LaserHit, Start, LaserEnd, ECC_Visibility, QueryParams);
	if (LaserHit.bBlockingHit) Distance = (LaserHit.ImpactPoint - Start).Size();
	
	FRotator CapsuleRotation = NewRot;
	CapsuleRotation.Pitch -= 90.f;
	Proprietor->GetLaserCollision()->SetWorldLocation(Start + LaserForward * Distance * 0.45f);
	Proprietor->GetLaserCollision()->SetWorldRotation(CapsuleRotation);
	Proprietor->GetLaserCollision()->SetCapsuleSize(25.f, Distance * 0.5f);
	
	Proprietor->GetLaser()->SetFloatParameter(TEXT("_Length"), Distance * 0.0007f);
	LaserTime += 0.025f;
	if (LaserTime < 3.f) return;
	StopLaser();
}
