#include "Samurai.h"
#include "NiagaraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "PossessionGame/Destructibles/Destructible.h"
#include "PossessionGame/Enums/ECameraShake.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Enums/EPostProcessMaterial.h"
#include "PossessionGame/Enums/EWeapon.h"
#include "PossessionGame/Projectiles/Throwables/SamuraiShuriken.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
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
	bIsInputLocked = true;
	bIsLookLocked = true;
	AssetsManager->PlaySound2D(ESound::SamuraiDashSlash);
	if (!DashSlashVfx) AssetsManager->AttachVfx(EVfx::SamuraiDashSlash, [this]() -> bool { return GetIsAlive() && bIsDashSlashing && bIsHijacked; }, &DashSlashVfx, GetWeapon()->GetHandsMesh(), FName("DashSlashSocket"), GetVariation());
	else AssetsManager->ActivateWithVariation(EVfx::SamuraiDashSlash, nullptr, DashSlashVfx, false, GetVariation());
	if (!DashSlashLinesVfx) AssetsManager->AttachVfx(EVfx::SamuraiDashLines, [this]() -> bool { return GetIsAlive() && bIsDashSlashing && bIsHijacked; }, &DashSlashLinesVfx, Camera, NAME_None, GetVariation());
	else AssetsManager->ActivateWithVariation(EVfx::SamuraiDashLines, nullptr, DashSlashLinesVfx, false, GetVariation());
	P_SetDashMaterialValue(1.f);
	FVector DashDir = GetLastMovementInputVector();
	if (DashDir.IsNearlyZero()) DashDir = GetActorForwardVector();
	GetCharacterMovement()->GroundFriction = 0.f;
	GetCharacterMovement()->BrakingFrictionFactor = 0.f;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Destructible, ECR_Ignore);
	LaunchCharacter(DashDir.GetSafeNormal() * P_DashStrength, true, false);
	GetWorldTimerManager().SetTimer(DashOverlapTickHandle, this, &ASamurai::P_DashOverlapTick, DashOverlapCheckTick, true);
}

void ASamurai::P_EndDash()
{
	bIsInvincible = false;
	bIsActionInMotion = false;
	bIsDashSlashing = false;
	bIsInputLocked = false;
	bIsLookLocked = false;
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
	bIsActionInMotion = bState;
	if (bDeflectState)
	{
		SM_DeflectKatana->SetVisibility(true);
		AssetsManager->PlaySound2D(ESound::SamuraiDeflectActivate);
		if (!DeflectParticlesVfx) AssetsManager->AttachVfx(EVfx::SamuraiDeflectParticles, [this]() -> bool { return GetIsAlive() && bDeflectState && bIsHijacked; }, &DeflectParticlesVfx, Camera, NAME_None, GetVariation());
		else AssetsManager->ActivateWithVariation(EVfx::SamuraiDeflectParticles, nullptr, DeflectParticlesVfx, false, GetVariation());
	}
	else
	{
		SM_DeflectKatana->SetVisibility(false);
		AssetsManager->PlaySound2D(ESound::SamuraiDeflectDeactivate);
		if (DeflectParticlesVfx) DeflectParticlesVfx->DeactivateImmediate();
	}
	if (bState) P_ToggleRun(false);
	bCanRun = !bState;
	Cast<USamuraiHandsAnimInstance>(GetWeapon()->GetHandsAnimInstance())->SetDeflectState(bState);
}

void ASamurai::P_ThrowShuriken()
{
	const TObjectPtr<ASamuraiShuriken> Shuriken = Cast<ASamuraiShuriken>(GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(EPoolObject::SamuraiShuriken, Weapon->GetHandsSocketLocation(FName("ShurikenSocket")), Weapon->GetHandsSocketRotation(FName("ShurikenSocket"))));
	if (!Shuriken) return;
	Shuriken->InitWithProprietor(this);
	AssetsManager->PlaySound2D(ESound::SamuraiShurikenThrow);
	Shuriken->Throw();
}

ASamurai::ASamurai()
{
	SpawnSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("SpawnSfx"));
	SpawnSfx->SetAutoActivate(false);
	SpawnSfx->SetupAttachment(GetMesh());
	DeathSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("DeathSfx"));
	DeathSfx->SetAutoActivate(false);
	DeathSfx->SetupAttachment(GetMesh());
	DeflectSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("DeflectSfx"));
	DeflectSfx->SetAutoActivate(false);
	DeflectSfx->SetupAttachment(GetMesh());
	Sounds.Add(ESound::SamuraiSpawn, SpawnSfx);
	Sounds.Add(ESound::SamuraiDeath, DeathSfx);
	Sounds.Add(ESound::SamuraiDeflect, DeflectSfx);
	SM_DeflectKatana = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DeflectKatana"));
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
		AssetsManager->AttachVfx(EVfx::AcoEject, nullptr, nullptr, GetMesh(), FName("DeathSocket"), EVariation::Default,
		false, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false, 
		{{FName("Skeletal Mesh"), FNiagaraAttribute(EUserParamType::Object, GetMesh())}});
	}
}

void ASamurai::AI_TriggerDeathVoiceLine() { AssetsManager->PlaySound2D(ESound::SamuraiOnHijack); }

void ASamurai::AI_PlayDeathSound()
{
	Super::AI_PlayDeathSound();
	PlaySound(ESound::SamuraiDeath);
}

void ASamurai::AI_PlaySpawnSound() { PlaySound(ESound::SamuraiSpawn, true); }

void ASamurai::P_Setup()
{
	Super::P_Setup();
	DelegateManager->OnSamuraiDashSlashEnd.BindUObject(this, &ASamurai::P_EndDash);
	if (!SM_DeflectKatana || !GetWeapon() || !GetWeapon()->GetHandsMesh()) return;
	SM_DeflectKatana->AttachToComponent(GetWeapon()->GetHandsMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("DeflectWeaponSocket"));
	SamuraiHat->DestroyThis();
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
	for (FHitResult& Hit : Hits)
	{
		TObjectPtr<AActor> HitActor = Hit.GetActor();
		if (HitActor && !DashHitActors.Contains(HitActor))
		{
			DashHitActors.Add(HitActor);
			UGameplayStatics::ApplyDamage(HitActor, P_DashDamage, GetController(), this, nullptr);
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

float ASamurai::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bDeflectState && DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent* PointDamage = static_cast<const FPointDamageEvent*>(&DamageEvent);
		FVector ShotDirection = PointDamage->ShotDirection;
		ShotDirection.Z = 0;
		ShotDirection.Normalize();
		FVector Forward = GetActorForwardVector();
		Forward.Z = 0;
		Forward.Normalize();
		if (FVector::DotProduct(Forward, ShotDirection) > 0.f)
		{
			PlaySound(ESound::SamuraiDeflect);
			PlayDeflectionAnim();
			return 0.f;
		}
	}
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void ASamurai::BeginPlay()
{
	CharacterName = ECharacterType::Samurai;
	Super::BeginPlay();
	SetupSamuraiHat();
}

void ASamurai::SetupSamuraiHat()
{
	SamuraiHat = Cast<ADestructible>(GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(EPoolObject::SamuraiHat));
	SamuraiHat->InitWithProprietor(this);
	if (const TObjectPtr<UStaticMeshComponent> HatMesh = SamuraiHat->GetMesh()) HatMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	AttachDestructible(SamuraiHat, "HatSocket");
}

void ASamurai::HandleDeactivation()
{
	if (!SamuraiHat->GetIsDeactivated()) SamuraiHat->HandleDeactivation();
	Super::HandleDeactivation();
}

void ASamurai::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	if (!AI || AI->GetIsStateTreeRunning()) return;
	AI->SetIsInJumpMode(false);
}

void ASamurai::PlayDeflectionAnim()
{
	constexpr EHandsAnimState DeflectAnims[] = { EHandsAnimState::Deflect1, EHandsAnimState::Deflect2, EHandsAnimState::Deflect3, EHandsAnimState::Deflect4 };
	int32 Index = FMath::RandRange(0, 2);
	if (LastDeflectAnim != EHandsAnimState::None)
	{
		if (Index >= static_cast<int32>(LastDeflectAnim) - static_cast<int32>(EHandsAnimState::Deflect1)) Index++;
	}
	const EHandsAnimState NewAnim = DeflectAnims[Index];
	LastDeflectAnim = NewAnim;
	UE_LOG(LogTemp, Error, TEXT("%s"), *UEnum::GetValueAsString(LastDeflectAnim));
	GetWeapon()->GetHandsAnimInstance()->PlayMontage(NewAnim);
}
