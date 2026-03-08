#include "Tank.h"
#include "EnhancedInputComponent.h"
#include "EnhancedPlayerInput.h"
#include "InputMappingContext.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Enums/ECameraShake.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Projectiles/Throwables/GravitationalPull.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"
#include "PossessionGame/Weapons/Character Weapons/TankWeapon.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/Weapon.h"
#include "PossessionGame/Weapons/Management/WeaponAnimInstance.h"

void ATank::P_Shoot()
{
	if (bIsInputLocked || !Weapon->P_CanShoot()) return;
	Weapon->P_Shoot(EPoolObject::TankProjectile);
}

void ATank::P_InitiateThrowGravitationalPull()
{
	const TObjectPtr<AGravitationalPull> GravitationalPull = Cast<AGravitationalPull>(GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(EPoolObject::TankGravitationalPull, Weapon->GetHandsSocketLocation(FName("ThrowSocket")), Weapon->GetHandsSocketRotation(FName("ThrowSocket"))));
	if (!GravitationalPull) return;
	AssetsManager->PlaySound2D(ESound::TankGravitationalBallLaunch);
	GravitationalPull->InitWithProprietor(this);
	GravitationalPull->Throw();
	if (!Weapon) return;
	//Weapon->
}

ATank::ATank()
{
	SpawnSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("SpawnSfx"));
	SpawnSfx->SetAutoActivate(false);
	SpawnSfx->SetupAttachment(GetMesh());
	DeathSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("DeathSfx"));
	DeathSfx->SetAutoActivate(false);
	DeathSfx->SetupAttachment(GetMesh());
	Sounds.Add(ESound::TankSpawn, SpawnSfx);
	Sounds.Add(ESound::TankDeath, DeathSfx);
}

void ATank::AI_ReloadLogic() { if (!bIsActionInMotion && Weapon->GetIsReloadEnabled()) AI_Reload(); }

void ATank::AI_SetCanShoot(const bool bState) { bCanShoot = bState; }

void ATank::AI_PlayShootMontage()
{
	if (!bShootPauseActive && Weapon->GetAnimInstance()->IsMontageExisting(EWeaponAnimState::Shoot) && !Weapon->GetAnimInstance()->IsPlaying(EWeaponAnimState::Shoot))
	{
		Weapon->GetAnimInstance()->PlayMontage(EWeaponAnimState::Shoot);
		AI_SetCanShoot(true);
	}
}

void ATank::AI_SetupWeapon()
{
	Weapon = GetWorld()->GetGameInstance()->GetSubsystem<UWeaponManager>()->GetWeapon(GetWorld(), EWeapon::TankWeapon, GetMesh());
	Super::AI_SetupWeapon();
}

void ATank::AI_TriggerDeathVoiceLine() { AssetsManager->PlaySound2D(ESound::TankOnHijack); }

void ATank::AI_PlaySpawnSound() { PlaySound(ESound::TankSpawn, true); }

void ATank::AI_PlayDeathSound()
{
	Super::AI_PlayDeathSound();
	PlaySound(ESound::TankDeath);
}

bool ATank::P_GetIsLeftOrRightInputNotPressed()
{
	return !P_InputManager->IsPressed(FKey("LeftMouseButton")) && !P_InputManager->IsPressed(FKey("RightMouseButton"));
}

void ATank::P_Setup()
{
	DelegateManager->OnWeaponAmmoChangeOneParam.AddUObject(this, &AEnemy::P_UpdateAmmoHUD);
	Super::P_Setup();
	Weapon->GetHandsAnimInstance()->OnHandsMontageEnd.BindUObject(this, &ATank::SetIsActionInMotion);
}

void ATank::P_SetupLeftClickInputStartedState()
{
	Super::P_SetupLeftClickInputStartedState();
	EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(8).Action, ETriggerEvent::Started, this, &ATank::P_ToggleChargeMinigun, true);
}

void ATank::P_SetupLeftClickInputCompletedState()
{
	Super::P_SetupLeftClickInputCompletedState();
	EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(8).Action, ETriggerEvent::Completed, this, &ATank::P_ToggleChargeMinigun, false);
}

void ATank::P_Destroy()
{
	Super::P_Destroy();
	Weapon->AttachToSocket(GetMesh());
}

void ATank::P_ToggleChargeMinigun(const bool bState)
{
	if (Weapon->GetCurrentAmmoAmount() <= 0) return;
	if (bState)
	{
		if (!Weapon->GetAnimInstance()->IsPlaying(EWeaponAnimState::ChargeUp) && !Weapon->GetAnimInstance()->IsPlaying(EWeaponAnimState::Shoot))
		{
			AssetsManager->RemoveStoredSound(ESound::TankWeaponChargeDown);
			AssetsManager->PlaySound2D(ESound::TankWeaponChargeUp, true);
			Weapon->GetAnimInstance()->PlayMontage(EWeaponAnimState::ChargeUp);
		}
	}
	else P_StopChargingMinigun();
}

void ATank::P_StopChargingMinigun()
{
	AssetsManager->RemoveStoredSound(ESound::TankWeaponChargeUp);
	AssetsManager->PlaySound2D(ESound::TankWeaponChargeDown, true);
	if (Weapon->GetAnimInstance()->IsPlaying(EWeaponAnimState::ChargeUp)) Weapon->GetAnimInstance()->StopMontage(EWeaponAnimState::ChargeUp);
	else if (P_GetIsLeftOrRightInputNotPressed()) Weapon->GetAnimInstance()->StopMontage(EWeaponAnimState::Shoot);
}

void ATank::P_WeaponShoot() { Weapon->P_Shoot(EPoolObject::TankProjectile); }

void ATank::P_ProcessSlamDecal()
{
	if (!MID_Slam) return;
	DecalDissolveValue += 0.1f;
	MID_Slam->SetScalarParameterValue(FName("RadialDissolve"), DecalDissolveValue);
	if (++DecalLoopsCount >= 10)
	{
		GetWorldTimerManager().ClearTimer(SlamDecalHandle);
		DecalDissolveValue = 0.f;
		DecalLoopsCount = 0;
	}
}

void ATank::P_MakeCameraShake(const ECameraShake InCameraShakeName)
{
	if (InCameraShakeName == ECameraShake::TankSlamCameraShake) UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraShake(SlamCameraShake);
	else Super::P_MakeCameraShake(InCameraShakeName);
}

void ATank::P_StopMinigun()
{
	if (Weapon->GetAnimInstance()->IsPlaying(EWeaponAnimState::ChargeUp)) Weapon->GetAnimInstance()->StopMontage(EWeaponAnimState::ChargeUp);
	else Weapon->GetAnimInstance()->StopMontage(EWeaponAnimState::Shoot);
}

void ATank::BeginPlay()
{
	CharacterName = ECharacterType::Tank;
	bCanReload = false;
	Super::BeginPlay();
	if (MI_Slam) MID_Slam = UMaterialInstanceDynamic::Create(MI_Slam, this);
	Cast<ATankWeapon>(Weapon)->OnAmmoEnded.BindUObject(this, &ATank::P_StopMinigun);
}

void ATank::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	if (!Weapon) return;
	const TObjectPtr<UHandsAnimInstance> HandsAnimInstance = Weapon->GetHandsAnimInstance();
	if (!HandsAnimInstance || !HandsAnimInstance->IsPlaying(EHandsAnimState::JumpSecondary)) return;
	P_MakeCameraShake(ECameraShake::TankSlamCameraShake);
	AssetsManager->PlaySound2D(ESound::TankSlam);
	if (!SlamVfx) AssetsManager->AttachVfx(EVfx::TankSlam, [this]() -> bool { return GetIsAlive() && bIsHijacked; }, &SlamVfx, GetMesh(), FName("RootSocket"));
	else SlamVfx->Activate(true);
	if (MID_Slam)
	{
		UGameplayStatics::SpawnDecalAtLocation(GetWorld(), MID_Slam, DecalSize, Hit.ImpactPoint, Hit.ImpactNormal.Rotation(), DecalLifespan);
		GetWorldTimerManager().SetTimer(SlamDecalHandle, this, &ATank::P_ProcessSlamDecal, 0.025f, true);
	}
	HandsAnimInstance->PlayMontage(EHandsAnimState::Slam);
	P_StompImpact(GetSocketLocation("RootSocket"));
}
