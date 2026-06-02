#include "Tank.h"
#include "EnhancedInputComponent.h"
#include "EnhancedPlayerInput.h"
#include "InputMappingContext.h"
#include "Components/AudioComponent.h"
#include "Engine/OverlapResult.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Enums/ECameraShake.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Projectiles/Throwables/GravitationalPull.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/SaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"
#include "PossessionGame/Weapons/Character Weapons/TankWeapon.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/Weapon.h"
#include "PossessionGame/Weapons/Management/WeaponAnimInstance.h"

void ATank::AI_SpawnShield()
{
	AI_Shield = GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(EPoolObject::AI_TankShield);
	if (!AI_Shield) return;
	AI_Shield->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("DeathSocket"));
	AI_Shield->InitWithProprietor(this);
	if (SaveSlotManager->GetRunSaveSlot()->IsSpecialUpgradeAvailable(EUpgrade::TankAbility1, EUpgradeAttribute::Special1))
	{
		GetWorldTimerManager().SetTimer(AI_ShieldScalingHandle, this, &ATank::AI_ProcessShieldScaling, 0.2f, true);
	}
}

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
	Cast<ATankWeapon>(Weapon)->P_BallRelease();
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

void ATank::SetInputState(const bool bIsLocked)
{
	Super::SetInputState(bIsLocked);
	if (!P_InputManager) return;
	if (!bIsLocked && P_InputManager->IsPressed(FKey("LeftMouseButton"))) P_ToggleChargeMinigun(true);
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

void ATank::AI_ReceivedDamage(const EDamageReason InDamageReason, const float InDamage)
{
	if (CanAbilityBeExecuted(0) && !IsActionsDisabled() && !bIsActionInMotion && CurrentHp <= MaxHp * 0.5f && CurrentHp > 0) AI_UseAbility(0);
	Super::AI_ReceivedDamage(InDamageReason, InDamage);
}

void ATank::AI_ProcessShieldScaling()
{
	if (++AI_ShieldScalingLoopsCount > 25)
	{
		AI_ResetShield();
		return;
	}
	if (!AI_Shield) return;
	AI_ShieldScale += 0.06f;
	AI_Shield->SetActorScale3D(FVector(AI_ShieldScale));
}

void ATank::AI_ResetShield()
{
	GetWorldTimerManager().ClearTimer(AI_ShieldScalingHandle);
	AI_ShieldScale = 3.f;
	AI_ShieldScalingLoopsCount = 0;
}

void ATank::AI_AddLegendaryUpgrade(const EUpgrade InUpgradeName, const EUpgradeAttribute InUpgradeAttribute)
{
	if (InUpgradeAttribute != EUpgradeAttribute::Special1) return;
	if (InUpgradeName == EUpgrade::TankAbility2)
	{
		bIsSpecialDamageReductionUpgradeTaken = true;
		DamageReduction = FMath::Max(0.f, DamageReduction - 0.3f);
	}
	else if (InUpgradeName == EUpgrade::TankAbility3) bIsSpecialIgnoreNegativeEffectsUpgradeTaken = true;
	if (bIsSpecialDamageReductionUpgradeTaken && bIsSpecialIgnoreNegativeEffectsUpgradeTaken) DelegateManager->OnSpecialUpgradeAdded.RemoveAll(this);
}

void ATank::AI_ReceiveStunEffect(const bool bHitByOtherAI, const float InEffectDuration, const float InDamage, FVector InHitDirection)
{
	if (bIsSpecialIgnoreNegativeEffectsUpgradeTaken) return;
	Super::AI_ReceiveStunEffect(bHitByOtherAI, InEffectDuration, InDamage, InHitDirection);
}

void ATank::AI_ReceiveEmpEffect(const float InEffectDuration)
{
	if (bIsSpecialIgnoreNegativeEffectsUpgradeTaken) return;
	Super::AI_ReceiveEmpEffect(InEffectDuration);
}

void ATank::AI_PlayDeathElectrification()
{
	// LACKS DEATH ELECTRIFICATION 	else AssetsManager->AttachVfx(EVfx::RangerDeathElectrification, nullptr, nullptr, GetMesh(), FName("DeathSocket"));
	if (bIsAco)
	{
		AssetsManager->AttachVfx(EVfx::AcoEject, nullptr, nullptr, GetMesh(), FName("DeathSocket"), EVariation::Default,
		false, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false, 
		{{FName("Skeletal Mesh"), FNiagaraAttribute(EUserParamType::Object, GetMesh())}});
	}
}

bool ATank::P_GetIsLeftMouseInputNotPressed() const
{
	if (bIsInputLocked) return true;
	return !P_InputManager->IsPressed(FKey("LeftMouseButton"));
}

float ATank::P_GetStompDamage()
{
	return StompDamage + GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetUpgradeAttributeValues(EUpgrade::TankAbility3, EUpgradeAttribute::FlatDamage, true) + StompDamage * GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetPlayerLevelAllGlobalDamageBonus();
}

float ATank::P_GetStompSize()
{
	return Super::P_GetStompSize() * (1.f + GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetUpgradeAttributeValues(EUpgrade::TankAbility3, EUpgradeAttribute::Size, true) * 0.01f);
}

void ATank::P_Setup()
{
	DelegateManager->OnWeaponAmmoChangeOneParam.AddUObject(this, &AEnemy::P_UpdateAmmoHUD);
	if (!bIsSpecialDamageReductionUpgradeTaken) DelegateManager->OnSpecialUpgradeAdded.RemoveAll(this);
	Super::P_Setup();
	Weapon->GetHandsAnimInstance()->OnHandsMontageEnd.BindUObject(this, &ATank::SetIsActionInMotion);
	if (!AI_Shield) return;
	AI_ResetShield();
	GetWorld()->GetSubsystem<UPoolingManager>()->ReturnObjectToPool(EPoolObject::AI_TankShield, AI_Shield);
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

void ATank::P_StopChargingMinigun()
{
	AssetsManager->RemoveStoredSound(ESound::TankWeaponChargeUp);
	AssetsManager->PlaySound2D(ESound::TankWeaponChargeDown, true);
	if (Weapon->GetAnimInstance()->IsPlaying(EWeaponAnimState::ChargeUp)) Weapon->GetAnimInstance()->StopMontage(EWeaponAnimState::ChargeUp);
	else if (P_GetIsLeftMouseInputNotPressed()) Weapon->GetAnimInstance()->StopMontage(EWeaponAnimState::Shoot);
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

void ATank::P_LegendarySlamStun(const FVector& InSlamLocation)
{
	TArray<FOverlapResult> Overlaps;
	GetWorld()->OverlapMultiByProfile(Overlaps, InSlamLocation, FQuat::Identity, FName("TriggerPlayer"), FCollisionShape::MakeSphere(P_GetStompSize() * LegendaryStunRadiusMultiplier));
	for (const FOverlapResult& Result : Overlaps)
	{
		TObjectPtr<AActor> Actor = Result.GetActor();
		if (!Actor || !Actor->ActorHasTag("AliveEnemy")) continue;
		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		Params.AddIgnoredActor(Actor);
		if (GetWorld()->LineTraceSingleByChannel(Hit, InSlamLocation, Actor->GetActorLocation(), ECC_Visibility, Params)) continue;
		if (const TObjectPtr<AEnemy> Enemy = Cast<AEnemy>(Actor))
		{
			UE_LOG(LogTemp, Error, TEXT("ENEMY GOT STUNNED: %s"), *Actor->GetName());
			Enemy->AI_ReceiveStunEffect(false, LegendaryStunDuration, -1.f, FVector(NAN, NAN, NAN));
		}
	}
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
	bIsSpecialDamageReductionUpgradeTaken = SaveSlotManager->GetRunSaveSlot()->IsSpecialUpgradeAvailable(EUpgrade::TankAbility2, EUpgradeAttribute::Special1);
	if (bIsSpecialDamageReductionUpgradeTaken) DamageReduction = FMath::Max(0.f, DamageReduction - 0.3f);
	bIsSpecialIgnoreNegativeEffectsUpgradeTaken = SaveSlotManager->GetRunSaveSlot()->IsSpecialUpgradeAvailable(EUpgrade::TankAbility3, EUpgradeAttribute::Special1);
	if (!bIsSpecialDamageReductionUpgradeTaken || !bIsSpecialIgnoreNegativeEffectsUpgradeTaken) DelegateManager->OnSpecialUpgradeAdded.AddUObject(this, &ATank::AI_AddLegendaryUpgrade);
}

void ATank::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	if (!Weapon) return;
	const TObjectPtr<UHandsAnimInstance> HandsAnimInstance = Weapon->GetHandsAnimInstance();
	if (!HandsAnimInstance || !HandsAnimInstance->IsPlaying(EHandsAnimState::JumpSecondary)) return;
	P_MakeCameraShake(ECameraShake::TankSlamCameraShake);
	AssetsManager->PlaySound2D(ESound::TankSlam);
	if (!SlamVfx) AssetsManager->AttachVfx(EVfx::TankSlam, [this]() -> bool { return GetIsAlive() && bIsHijacked; }, &SlamVfx, GetMesh(), FName("RootSocket"), GetVariation());
	else AssetsManager->ActivateWithVariation(EVfx::TankSlam, nullptr, SlamVfx, false, GetVariation());
	if (MID_Slam)
	{
		const float StompSize = P_GetStompSize();
		UGameplayStatics::SpawnDecalAtLocation(GetWorld(), MID_Slam, FVector(10.f, StompSize, StompSize), Hit.ImpactPoint, Hit.ImpactNormal.Rotation(), DecalLifespan);
		GetWorldTimerManager().SetTimer(SlamDecalHandle, this, &ATank::P_ProcessSlamDecal, 0.025f, true);
	}
	HandsAnimInstance->PlayMontage(EHandsAnimState::Slam);
	const FVector SlamLocation = GetSocketLocation("RootSocket");
	P_StompImpact(SlamLocation);
	if (SaveSlotManager->GetRunSaveSlot()->IsSpecialUpgradeAvailable(EUpgrade::TankAbility3, EUpgradeAttribute::Special1)) P_LegendarySlamStun(SlamLocation);
}

void ATank::InitiateDeath()
{
	if (!bIsSpecialDamageReductionUpgradeTaken) DelegateManager->OnSpecialUpgradeAdded.RemoveAll(this);
	if (AI_Shield)
	{
		AI_ResetShield();
		GetWorld()->GetSubsystem<UPoolingManager>()->ReturnObjectToPool(EPoolObject::AI_TankShield, AI_Shield);
	}
	Super::InitiateDeath();
}

void ATank::SetActionSpeed(const float InSpeed)
{
	if (bIsSpecialIgnoreNegativeEffectsUpgradeTaken || InSpeed > 1.f) return;
	Super::SetActionSpeed(InSpeed);
}
