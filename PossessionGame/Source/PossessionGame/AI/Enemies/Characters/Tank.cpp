#include "Tank.h"
#include "EnhancedInputComponent.h"
#include "EnhancedPlayerInput.h"
#include "InputMappingContext.h"
#include "Components/AudioComponent.h"
#include "PossessionGame/Enums/ECharacterType.h"
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
	GravitationalPull->InitWithProprietor(this);
	GravitationalPull->Throw();
}

void ATank::P_UseShield()
{
	if (!Weapon) return;
	const TObjectPtr<USkeletalMeshComponent> HandsMesh = Weapon->GetHandsMesh();
	if (!HandsMesh) return;
	for (int32 i = 0; i < ShieldPartsAmount; i++)
	{
		TObjectPtr<UStaticMeshComponent> ShieldMeshComponent = NewObject<UStaticMeshComponent>(this);
		ShieldMeshComponent->RegisterComponent();
		ShieldMeshComponent->SetStaticMesh(ShieldMesh);
		ShieldMeshComponent->SetComponentTickEnabled(false);
		//WeakSpotMeshComponent->ComponentTags.Add("WeakSpot");
		ShieldMeshComponent->AttachToComponent(HandsMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, FName("ShieldSocket"));

		const float Angle = -2.f * PI * (i % 10) / ShieldPartsAmount;
		float Z = 5.f * sin(Angle);
		float BendRadius = ShieldPartXOffset * 5;
		float Theta = i / 10 / 4.f * (PI / 4.f);
		float X = BendRadius * sin(Theta);

		FVector Position(X, ShieldPartYOffset * (i % 10), Z * ShieldPartZOffset);
		UE_LOG(LogTemp, Error, TEXT("POSITION: %s"), *Position.ToString());
		

		// Orient object to face outward (normal at that point)
		//ShieldMeshComponent->SetRelativeLocation(Position);
		ShieldMeshComponent->AddRelativeLocation(Position);
		FVector Normal = Position.GetSafeNormal();
		FRotator Rotation = Normal.Rotation();
		ShieldMeshComponent->SetRelativeRotation(Rotation);
		//WeakSpotMeshComponent->SetCollisionProfileName(FName("WeakSpot"));
		//WeakSpotMeshComponent->SetVisibility(false);
		ShieldMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//WeakSpotMeshComponent->SetWorldScale3D(FVector(0.25f));
		ShieldParts.Add(ShieldMeshComponent, 10.f);
	}
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
	//if (P_GetIsLeftOrRightInputNotPressed() || bState) SetInitialWalkSpeed(!bState);
}

void ATank::P_StopChargingMinigun()
{
	AssetsManager->RemoveStoredSound(ESound::TankWeaponChargeUp);
	AssetsManager->PlaySound2D(ESound::TankWeaponChargeDown, true);
	if (Weapon->GetAnimInstance()->IsPlaying(EWeaponAnimState::ChargeUp)) Weapon->GetAnimInstance()->StopMontage(EWeaponAnimState::ChargeUp);
	else if (P_GetIsLeftOrRightInputNotPressed()) Weapon->GetAnimInstance()->StopMontage(EWeaponAnimState::Shoot);
}

void ATank::P_WeaponShoot() { Weapon->P_Shoot(EPoolObject::TankProjectile); }

void ATank::P_StopMinigun()
{
	if (Weapon->GetAnimInstance()->IsPlaying(EWeaponAnimState::ChargeUp)) Weapon->GetAnimInstance()->StopMontage(EWeaponAnimState::ChargeUp);
	else Weapon->GetAnimInstance()->StopMontage(EWeaponAnimState::Shoot);
	//SetInitialWalkSpeed(true);
}

void ATank::BeginPlay()
{
	CharacterName = ECharacterType::Tank;
	bCanReload = false;
	Super::BeginPlay();
	Cast<ATankWeapon>(Weapon)->OnAmmoEnded.BindUObject(this, &ATank::P_StopMinigun);
}
