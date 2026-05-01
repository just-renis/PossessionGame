#include "Samurai.h"
#include "NiagaraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "PossessionGame/Destructibles/Destructible.h"
#include "PossessionGame/Enums/ECameraShake.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EPostProcessMaterial.h"
#include "PossessionGame/Enums/EWeapon.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PostProcessManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"
#include "PossessionGame/Weapons/Animation Instances/Hands/SamuraiHandsAnimInstance.h"
#include "PossessionGame/Weapons/Character Weapons/SamuraiWeapon.h"

void ASamurai::AI_SlashAttack()
{
	SetIsAnimActionInMotion(true);
	GetAnimInstance()->PlayMontage(EEnemyAnimState::Slash);
	if (Weapon) Weapon->AI_Shoot();
}

void ASamurai::P_DashSlash()
{
	bIsInvincible = true;
	bIsDashSlashing = true;
	if (!DashSlashVfx) AssetsManager->AttachVfx(EVfx::SamuraiDashSlash, [this]() -> bool { return GetIsAlive() && bIsDashSlashing && bIsHijacked; }, &DashSlashVfx, GetWeapon()->GetHandsMesh(), FName("DashSlashSocket"));
	else DashSlashVfx->Activate(true);
	if (!DashSlashLinesVfx) AssetsManager->AttachVfx(EVfx::SamuraiDashLines, [this]() -> bool { return GetIsAlive() && bIsDashSlashing && bIsHijacked; }, &DashSlashLinesVfx, Camera);
	else DashSlashLinesVfx->Activate(true);
	P_SetDashMaterialValue(1.f);
	FVector DashDir = GetLastMovementInputVector();
	if (DashDir.IsNearlyZero()) DashDir = GetActorForwardVector();
	GetCharacterMovement()->GroundFriction = 0.f;
	GetCharacterMovement()->BrakingFrictionFactor = 0.f;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Destructible, ECR_Ignore);
	LaunchCharacter(DashDir.GetSafeNormal() * DashStrength, true, false);
	GetWorldTimerManager().SetTimer(DashOverlapTickHandle, this, &ASamurai::P_DashOverlapTick, DashOverlapCheckTick, true);
}

void ASamurai::P_EndDash()
{
	bIsInvincible = false;
	bIsActionInMotion = false;
	bIsDashSlashing = false;
	if (DashSlashLinesVfx) DashSlashLinesVfx->DeactivateImmediate();
	P_SetDashMaterialValue(0.f);
	GetCharacterMovement()->GroundFriction = BrakingFrictionFactor;
	GetCharacterMovement()->BrakingFrictionFactor = GroundFriction;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Destructible, ECR_Block);
	GetWorldTimerManager().ClearTimer(DashOverlapTickHandle);
	DashHitActors.Empty();
}

void ASamurai::P_ChangeDeflectState(const bool bState)
{
	bDeflectState = bState;
	if (bDeflectState)
	{
		if (!DeflectParticlesVfx) AssetsManager->AttachVfx(EVfx::SamuraiDeflectParticles, [this]() -> bool { return GetIsAlive() && bDeflectState && bIsHijacked; }, &DeflectParticlesVfx, Camera);
		else DeflectParticlesVfx->Activate(true);
	}
	else if (DeflectParticlesVfx) DeflectParticlesVfx->DeactivateImmediate();
	if (bState) P_ToggleRun(false);
	bCanRun = !bState;
	Cast<USamuraiHandsAnimInstance>(GetWeapon()->GetHandsAnimInstance())->SetDeflectState(bState);
}

void ASamurai::AI_SetupWeapon()
{
	Weapon = GetWorld()->GetGameInstance()->GetSubsystem<UWeaponManager>()->GetWeapon(GetWorld(), EWeapon::SamuraiWeapon, GetMesh());
	Super::AI_SetupWeapon();
}

void ASamurai::AI_PlayDeathElectrification()
{
	// LACKS DEATH ELECTRIFICATION AssetsManager->AttachVfx(EVfx::GuardianDeathElectrification, nullptr, nullptr, GetMesh(), FName("DeathSocket"));
	if (bIsAco)
	{
		AssetsManager->AttachVfx(EVfx::AcoEject, nullptr, nullptr, GetMesh(), FName("DeathSocket"),
		false, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false, EVariation::Default,
		{{FName("Skeletal Mesh"), FNiagaraAttribute(EUserParamType::Object, GetMesh())}});
	}
}

void ASamurai::P_Setup()
{
	Super::P_Setup();
	//SamuraiHat->HandleDeactivation();
}

void ASamurai::P_Shoot()
{
	if (bIsInputLocked || bIsActionInMotion || !bCanShoot || !Weapon) return;
	Weapon->P_Shoot(EPoolObject::SamuraiSlashAttack);
}

void ASamurai::P_MakeCameraShake(const ECameraShake InCameraShakeName)
{
	if (InCameraShakeName == ECameraShake::SamuraiPlayerSlashCameraShake) UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraShake(SlashCameraShake);
	Super::P_MakeCameraShake(InCameraShakeName);
}

void ASamurai::P_DashOverlapTick()
{
	const FVector Start = GetActorLocation();
	const FVector End = Start + GetVelocity() * DashOverlapCheckTick;
	TArray<FHitResult> Hits;
	GetWorld()->SweepMultiByChannel(Hits, Start, End, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeBox(DashOverlapSweepBox));
//	FVector Center = (Start + End) / 2.f;
////	FVector Extent = FVector(DashOverlapSweepBox);
//	DrawDebugBox(GetWorld(), Center, Extent, FColor::Green, false, 5.f);
	for (FHitResult& Hit : Hits)
	{
		TObjectPtr<AActor> HitActor = Hit.GetActor();

		if (HitActor && !DashHitActors.Contains(HitActor))
		{
			DashHitActors.Add(HitActor);
			UGameplayStatics::ApplyDamage(HitActor, DashDamage, GetController(), this, nullptr);
		}
	}
}

void ASamurai::P_SetDashMaterialValue(const float InValue)
{
	if (!MID_Dash) MID_Dash = UMaterialInstanceDynamic::Create(MI_Dash, this);
	if (InValue > 0.1f)
	{
		MID_Dash->SetScalarParameterValue(FName("Density"), 2.3f);
		MID_Dash->SetScalarParameterValue(FName("Intensity"), 1.8f);
		MID_Dash->SetScalarParameterValue(FName("Radius"), 0.2f);
	}
	else
	{
		MID_Dash->SetScalarParameterValue(FName("Density"), 0.f);
		MID_Dash->SetScalarParameterValue(FName("Intensity"), 0.f);
		MID_Dash->SetScalarParameterValue(FName("Radius"), 0.f);
	}
	GetWorld()->GetSubsystem<UPostProcessManager>()->AddOrUpdateDynamicMaterial(EPostProcessMaterial::SpyTeleportEffect, MID_Dash, InValue);
}

void ASamurai::BeginPlay()
{
	CharacterName = ECharacterType::Samurai;
	Super::BeginPlay();
	//SetupSamuraiHat();
}

void ASamurai::SetupSamuraiHat()
{
	SamuraiHat = Cast<ADestructible>(GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(EPoolObject::SamuraiHat));
	SamuraiHat->Init(false);
	AttachDestructible(SamuraiHat, "HatSocket");
}

void ASamurai::HandleDeactivation()
{
	//if (!SamuraiHat->GetIsDeactivated()) SamuraiHat->HandleDeactivation();
	Super::HandleDeactivation();
}

void ASamurai::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	if (!AI || !AI->GetIsStateTreeRunning()) return;
	AI->SetIsInJumpMode(false);
}
