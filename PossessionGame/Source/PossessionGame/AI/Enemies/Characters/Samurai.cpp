#include "Samurai.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "PossessionGame/Destructibles/Destructible.h"
#include "PossessionGame/Enums/ECameraShake.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EWeapon.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
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
	GetWorld()->SweepMultiByChannel(
		Hits,
		Start,
		End,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeBox(DashOverlapSweepBox)
	);
	FVector Center = (Start + End) / 2.f;
	FVector Extent = FVector(DashOverlapSweepBox);
	DrawDebugBox(GetWorld(), Center, Extent, FColor::Green, false, 5.f);
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
