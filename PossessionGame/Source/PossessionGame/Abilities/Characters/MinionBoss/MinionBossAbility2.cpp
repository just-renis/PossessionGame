#include "MinionBossAbility2.h"
#include "NiagaraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/AI/Enemies/Animation Instances/MinionBossAnimationInstance.h"
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
	PlayerLocationAtInitiation = Proprietor->GetAI()->GetStateTreeData().GetTarget()->GetActorLocation();
	Cast<UMinionBossAnimationInstance>(Proprietor->GetAnimInstance())->SetIsActionMontagePlaying(true);
	UKismetSystemLibrary::K2_SetTimer(this, "ShootLaser", 1.65f, false);
}

bool UMinionBossAbility2::IsProprietorPossessed() { return Proprietor->GetIsPossessed(); }

void UMinionBossAbility2::SetProprietor(const TObjectPtr<AEnemy> AbilityProprietor)
{
	Proprietor = Cast<AMinionBoss>(AbilityProprietor);
	Proprietor->GetStopLaserDelegate().BindUObject(this, &UMinionBossAbility2::DisableLaser);
}

void UMinionBossAbility2::CooldownLogic(const float InDeltaTime)
{
	Super::CooldownLogic(InDeltaTime);
	if (CurrentChargesAmount > 0) Proprietor->EnableDistanceCheckForLaserEye();
}

void UMinionBossAbility2::StopLaser()
{
	DisableLaser();
	UKismetSystemLibrary::K2_ClearTimer(this, "UpdateLaserEndLocation");
	bLaserUpdateFirstFrame = true;
	LaserTime = 0.f;
	Proprietor->GetAnimInstance()->PlayMontage(EEnemyAnimState::StopLaser);
	UseCharge();
}

void UMinionBossAbility2::DisableLaser()
{
	bIsLaserDisabled = true;
	Proprietor->StopSound(ESound::MinionBossLaserEyeHold);
	if (Proprietor->GetLaser()) Proprietor->GetLaser()->Deactivate();
	if (Proprietor->GetLaserEyeBall()) Proprietor->GetLaserEyeBall()->Deactivate();
	Proprietor->GetLaserCollision()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void UMinionBossAbility2::ShootLaser()
{
	if (bIsLaserDisabled) return;
	Proprietor->EnableLaser();
	Proprietor->PlaySound(ESound::MinionBossLaserEyeHold);
	Proprietor->GetLaserCollision()->SetCapsuleSize(0.f, 0.f);
	Proprietor->GetLaserCollision()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	UKismetSystemLibrary::K2_SetTimer(this, "UpdateLaserEndLocation", 0.025f, true);
}

void UMinionBossAbility2::UpdateLaserEndLocation()
{
	if (!Proprietor || !Proprietor->GetLaser()) return;
	const TObjectPtr<AActor> Player = Proprietor->GetAI()->GetStateTreeData().GetTarget();
	if (!Player) return;
	// Laser calculation logic.
	const FVector Start = Proprietor->GetMesh()->GetSocketLocation(TEXT("LaserSocket"));
	FVector Direction = (Player->GetActorLocation() - Start).GetSafeNormal();
	const FRotator CurrentRotation = Proprietor->GetLaser()->GetComponentRotation();
	const FRotator DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(Direction.Rotation(), CurrentRotation);
	FRotator NewRotation = CurrentRotation;
	NewRotation.Yaw += FMath::Clamp(DeltaRotation.Yaw, -0.75f, 0.75f);
	NewRotation.Pitch += FMath::Clamp(DeltaRotation.Pitch, -0.75f, 0.75f);
	NewRotation.Roll = 0.f;
	if (bLaserUpdateFirstFrame)
	{
		Direction = (PlayerLocationAtInitiation - Start).GetSafeNormal();
		NewRotation = Direction.Rotation();
		NewRotation.Roll = 0.f;
		bLaserUpdateFirstFrame = false;
	}
	float Distance = Direction.Size();
	Direction /= Distance;
	Proprietor->GetLaser()->SetWorldRotation(NewRotation);
	// Laser blocking logic.
	const FVector LaserForward = NewRotation.Vector();
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Proprietor);
	FHitResult LaserHit;
	const FVector LaserEnd = Start + LaserForward * 10000.f;
	GetWorld()->LineTraceSingleByChannel(LaserHit, Start, LaserEnd, ECC_Visibility, QueryParams);
	if (LaserHit.bBlockingHit) Distance = (LaserHit.ImpactPoint - Start).Size();
	// Laser collision setup.
	NewRotation.Pitch -= 90.f;
	Proprietor->GetLaserCollision()->SetWorldLocation(Start + LaserForward * Distance * 0.45f);
	Proprietor->GetLaserCollision()->SetWorldRotation(NewRotation);
	Proprietor->GetLaserCollision()->SetCapsuleSize(35.f, Distance * 0.5f);
	// Laser length vfx change.
	Proprietor->GetLaser()->SetFloatParameter(TEXT("_Length"), Distance * 0.0007f);
	LaserTime += 0.025f;
	if (LaserTime < 3.f) return;
	StopLaser();
}
