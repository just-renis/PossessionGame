#include "Clone.h"
#include "CloneAnimationInstance.h"
#include "Components/CapsuleComponent.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/EnemyCommunicator.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"

AClone::AClone()
{
	MainCollision = CreateDefaultSubobject<UCapsuleComponent>("MainCollision");
	RootComponent = MainCollision;
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh");
	Mesh->SetupAttachment(RootComponent);
	FirstWeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("FirstWeaponMesh");
	SecondWeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("SecondWeaponMesh");
}

void AClone::InitClone(const bool bIsSpawnedByPlayer, const TObjectPtr<USkeletalMesh> InSkeletalMesh, const TObjectPtr<USkeletalMesh> InWeaponMesh)
{
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnEnemyDeathOccurred.AddUObject(this, &AClone::ChangeTarget);
	Velocity = FVector::ZeroVector;
	GetGameInstance()->GetSubsystem<UDelegateManager>()->DespawnPlayerClones.AddUObject(this, &AClone::StartDissolve);
	if (!Mesh->GetSkeletalMeshAsset())
	{
		Mesh->SetSkeletalMeshAsset(InSkeletalMesh);
		MID_Clone = UMaterialInstanceDynamic::Create(CloneMaterial, this);
		SetupMeshOptimization();
		AnimInstance = Cast<UCloneAnimationInstance>(Mesh->GetAnimInstance());
	}
	else Mesh->SetComponentTickEnabled(true);
	if (!FirstWeaponMesh->GetSkeletalMeshAsset() || !SecondWeaponMesh->GetSkeletalMeshAsset())
	{
		FirstWeaponMesh->SetSkeletalMeshAsset(InWeaponMesh);
		SecondWeaponMesh->SetSkeletalMeshAsset(InWeaponMesh);
		SetupWeaponsMeshesOptimization();
	}
	if (MID_Clone)
	{
		Mesh->SetMaterial(0, MID_Clone);
		FirstWeaponMesh->SetMaterial(0, MID_Clone);
		SecondWeaponMesh->SetMaterial(0, MID_Clone);
	}
	Super::InitClone(bIsSpawnedByPlayer, InSkeletalMesh, InWeaponMesh);
	Target = GetWorld()->GetSubsystem<UEnemyCommunicator>()->GetClosestEnemy(GetActorLocation());
	StartDeployment();
}

void AClone::DestroyThis()
{
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnEnemyDeathOccurred.RemoveAll(this);
	AnimInstance->SetIsEnabled(false);
	Mesh->SetComponentTickEnabled(false);
	GetGameInstance()->GetSubsystem<UDelegateManager>()->DespawnPlayerClones.RemoveAll(this);
	GetWorldTimerManager().ClearTimer(MoveHandle);
	GetWorldTimerManager().ClearTimer(ShootHandle);
	GetWorldTimerManager().ClearTimer(GravityHandle);
	GetWorld()->GetSubsystem<UPoolingManager>()->ReturnObjectToPool(EPoolObject::Clone, this);
}

void AClone::GravityLogic()
{
	const float Gravity = GetWorld()->GetGravityZ();
	Velocity.Z += Gravity * 0.02f;
	FHitResult Hit;
	AddActorWorldOffset(Velocity * 0.02f, true, &Hit);
	if (Hit.bBlockingHit) Velocity.Z = 0.f;
}

void AClone::SetupMeshOptimization() const
{
	Mesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	Mesh->bEnableUpdateRateOptimizations = true;
	Mesh->SkipUpdateOverlapsOptimEnabled = true;
	Mesh->SetAllowAnimCurveEvaluation(false);
	Mesh->bUseRefPoseOnInitAnim = true;
	Mesh->SetComponentTickInterval(1.f / 30.f);
}

void AClone::SetupWeaponsMeshesOptimization() const
{
	FirstWeaponMesh->SetComponentTickEnabled(false);
	SecondWeaponMesh->SetComponentTickEnabled(false);
	FirstWeaponMesh->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("AIWeaponSocket1"));
	SecondWeaponMesh->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("AIWeaponSocket2"));
}

void AClone::MoveTowardsTargetLogic()
{
	if (!Target)
	{
		if (!AnimInstance->GetIsIdleAnimationEnabled()) AnimInstance->SetIdleAnimationState(true);
		ChangeTarget(nullptr);
		return;
	}
	FVector Direction = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	Direction.Z = 0.f;
	AddActorWorldOffset(Direction * MovementSpeed * 0.02f, true);
	FRotator TargetRot = Direction.Rotation();
	TargetRot.Pitch = 0.f;
	TargetRot.Roll = 0.f;
	TargetRot.Yaw -= 90.f;
	SetActorRotation(TargetRot);
}

void AClone::ShootProjectile()
{
	FRotator BaseRotation;
	FVector BaseLocation;
	if (bFirstWeaponTurnToShoot)
	{
		BaseLocation = FirstWeaponMesh->GetSocketLocation("ProjectileSocket");
		BaseRotation = FirstWeaponMesh->GetSocketRotation("ProjectileSocket");
	}
	else
	{
		BaseLocation = SecondWeaponMesh->GetSocketLocation("ProjectileSocket");
		BaseRotation = SecondWeaponMesh->GetSocketRotation("ProjectileSocket");
	}
	bFirstWeaponTurnToShoot = !bFirstWeaponTurnToShoot;
	GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(EPoolObject::CloneProjectile, BaseLocation, BaseRotation)->InitCloneProjectile(ProjectileData, BaseRotation);
}

void AClone::ChangeTarget(const TObjectPtr<AActor>& InActorThatDied)
{
	if (InActorThatDied && Target != InActorThatDied) return;
	if (!GetWorld()->GetSubsystem<UEnemyCommunicator>()->GetIsEnemiesAvailable())
	{
		Target = nullptr;
		GetWorldTimerManager().ClearTimer(ShootHandle);
		return;
	}
	Target = GetWorld()->GetSubsystem<UEnemyCommunicator>()->GetClosestEnemy(GetActorLocation());
	if (AnimInstance->GetIsIdleAnimationEnabled())
	{
		AnimInstance->SetIdleAnimationState(false);
		GetWorldTimerManager().SetTimer(ShootHandle, this, &AClone::ShootProjectile, 0.37f, true);
	}
}

void AClone::StartDissolve()
{
	GetWorldTimerManager().SetTimer(DissolveHandle, this, &AClone::DissolveProcess, 0.025f, true);
}

void AClone::DissolveProcess() // 0.5 sec
{
	if (!MID_Clone) return;
	PatternMaxOpacity -= 0.025f;
	PatternOpacityDivisor -= 0.2f;
	DeploymentPercent -= 0.1f;
	MID_Clone->SetScalarParameterValue("PatternMaxOpacity", PatternMaxOpacity); 
	MID_Clone->SetScalarParameterValue("PatternOpacityDivisor", PatternOpacityDivisor);
	MID_Clone->SetScalarParameterValue("DeploymentPercent", DeploymentPercent);
	if (PatternOpacityDivisor > KINDA_SMALL_NUMBER) return;
	GetWorldTimerManager().ClearTimer(DissolveHandle);
	DestroyThis();
}

void AClone::StartDeployment()
{
	GetWorldTimerManager().SetTimer(DissolveHandle, this, &AClone::ProcessDeployment, 0.025f, true);
}

void AClone::ProcessDeployment() // 1 sec
{
	if (!MID_Clone) return;
	PatternMaxOpacity += 0.0125f;
	PatternOpacityDivisor += 0.1f;
	DeploymentPercent += 0.05f;
	MID_Clone->SetScalarParameterValue("PatternMaxOpacity", PatternMaxOpacity); 
	MID_Clone->SetScalarParameterValue("PatternOpacityDivisor", PatternOpacityDivisor);
	MID_Clone->SetScalarParameterValue("DeploymentPercent", DeploymentPercent);
	if (PatternOpacityDivisor < 2.f) return;
	AnimInstance->SetIsEnabled(true);
	GetWorldTimerManager().SetTimer(GravityHandle, this, &AClone::GravityLogic, 0.02f, true);
	GetWorldTimerManager().SetTimer(MoveHandle, this, &AClone::MoveTowardsTargetLogic, 0.02f, true);
	ShootProjectile();
	GetWorldTimerManager().SetTimer(ShootHandle, this, &AClone::ShootProjectile, 0.37f, true);
	GetWorldTimerManager().ClearTimer(DissolveHandle);
}
