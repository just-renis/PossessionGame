#include "Trickster.h"
#include "NavigationSystem.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StateTreeComponent.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EMoveMode.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PlayerDataManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"
#include "PossessionGame/Weapons/Character Weapons/TricksterWeapon.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"

ATrickster::ATrickster()
{
	AI_PoisonGrenadeTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("PoisonGrenadeLaunchTrigger"));
	AI_PoisonGrenadeTrigger->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);
	AI_PoisonGrenadeTrigger->SetCollisionProfileName("TriggerPlayer");
	AI_PoisonGrenadeTrigger->CanCharacterStepUpOn = ECB_No;
	AI_PoisonGrenadeTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AI_PoisonGrenadeTrigger->SetSphereRadius(PoisonGrenadeTriggerRadiusInMetres * 100.f);
	AI_PoisonGrenadeTrigger->OnComponentBeginOverlap.AddDynamic(this, &ATrickster::AI_OnPlayerEnterPoisonGrenadeTrigger);
	Sounds.Add(ESound::TricksterSpawn, CreateDefaultSubobject<UAudioComponent>(TEXT("spawnSound")));
	Sounds[ESound::TricksterSpawn]->SetAutoActivate(false);
	Sounds[ESound::TricksterSpawn]->SetupAttachment(GetMesh());
	Sounds.Add(ESound::TricksterDeath, CreateDefaultSubobject<UAudioComponent>(TEXT("deathSound")));
	Sounds[ESound::TricksterDeath]->SetAutoActivate(false);
	Sounds[ESound::TricksterDeath]->SetupAttachment(GetMesh());
	Sounds.Add(ESound::TricksterGrenadeThrow, CreateDefaultSubobject<UAudioComponent>(TEXT("grenadeThrowSound")));
	Sounds[ESound::TricksterGrenadeThrow]->SetAutoActivate(false);
	Sounds[ESound::TricksterGrenadeThrow]->SetupAttachment(GetMesh());
	Sounds.Add(ESound::RangerGrenadeLaunchAim, CreateDefaultSubobject<UAudioComponent>(TEXT("PoisonGrenadeLaunchAimSound")));
	Sounds[ESound::RangerGrenadeLaunchAim]->SetAutoActivate(false);
	Sounds[ESound::RangerGrenadeLaunchAim]->SetupAttachment(GetMesh());
	CloneUpgradeDashSound = CreateDefaultSubobject<UAudioComponent>(TEXT("CloneUpgradeDashSound"));
	CloneUpgradeDashSound->SetAutoActivate(false);
	CloneUpgradeDashSound->SetupAttachment(GetMesh());
	Sounds.Add(ESound::TricksterCloneDash, CloneUpgradeDashSound);
}

void ATrickster::AI_TogglePoisonGrenadeTrigger(const bool bState) const
{
	if (AI_PoisonGrenadeTrigger) AI_PoisonGrenadeTrigger->SetCollisionEnabled(bState ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
}

void ATrickster::AI_LaunchPoisonGrenade()
{
	if (bIsHijacked) return;
	PlaySound(ESound::TricksterGrenadeThrow, true);
	Weapon->ShootProjectile(EPoolObject::TricksterPoisonProjectile, true);
	Weapon->MuzzleFlashLogic();
}

void ATrickster::P_ThrowPoisonGrenade() const
{
	AssetsManager->PlaySound2D(ESound::TricksterGrenadeThrow);
	Weapon->ShootProjectile(EPoolObject::TricksterPoisonProjectile, true);
}

void ATrickster::P_ThrowEmpGrenade() const
{
	AssetsManager->PlaySound2D(ESound::TricksterGrenadeThrow);
	Weapon->ShootProjectile(EPoolObject::TricksterEmpProjectile, true, NAME_None, false);
}

void ATrickster::P_RemoveReloadSound() { AssetsManager->RemoveStoredSound(ESound::TricksterReload); }

TArray<FVector> ATrickster::GetAvailableCloneSpawnOffsets(int32 InClonesAmount)
{
	const TObjectPtr<UNavigationSystemV1> NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	TArray<FVector> ValidOffsets;
	if (!NavSys) return ValidOffsets;
	for (const FVector& Offset : CloneSpawnOffsets)
	{
		FNavLocation NavLocation;
		if (NavSys->ProjectPointToNavigation(GetActorLocation() + GetActorRotation().RotateVector(Offset), NavLocation, FVector(50.f, 50.f, 1000.f)))
		{
			NavLocation.Location.Z = GetActorLocation().Z;
			FHitResult Hit;
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(this);
			if (!GetWorld()->LineTraceSingleByChannel(Hit, GetActorLocation(), NavLocation.Location, ECC_WorldStatic, Params))
			{
				ValidOffsets.Add(NavLocation.Location);
				if (--InClonesAmount <= 0) return ValidOffsets;
			}
		}
	}
	return ValidOffsets;
}

void ATrickster::AI_SetupWeapon()
{
	Weapon = GetWorld()->GetGameInstance()->GetSubsystem<UWeaponManager>()->GetWeapon(GetWorld(), EWeapon::TricksterWeapon, GetMesh());
	Super::AI_SetupWeapon();
}

void ATrickster::AI_PlayDeathSound()
{
	Super::AI_PlayDeathSound();
	PlaySound(ESound::TricksterDeath);
}

void ATrickster::AI_TriggerDeathVoiceLine() { AssetsManager->PlaySound2D(ESound::TricksterOnHijack); }

void ATrickster::AI_PlaySpawnSound() { PlaySound(ESound::TricksterSpawn, true); }

void ATrickster::AI_PlayDeathElectrification()
{
	if (bIsAco)
	{
		AssetsManager->AttachVfx(EVfx::AcoEject, nullptr, nullptr, GetMesh(), FName("DeathSocket"),
		false, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false, EVariation::Default,
		{{FName("Skeletal Mesh"), FNiagaraAttribute(EUserParamType::Object, GetMesh())}});
	}
	else AssetsManager->AttachVfx(EVfx::TricksterDeathElectrification, nullptr, nullptr, GetMesh(), FName("DeathSocket"));
}

void ATrickster::AI_TryTriggeringPoisonGrenadeLaunch()
{
	if (!AI || AnimInstance->IsPlaying(EEnemyAnimState::Reload) || bIsActionInMotion) return;
	const TObjectPtr<UStateTreeComponent>& StateTree = AI->GetStateTreeData().GetStateTreeRef();
	if (!StateTree || !StateTree->IsRunning()) return;
	if (AI->GetStateTreeData().GetIsTargetInMySight())
	{
		AI->GetStateTreeData().TriggerEvent(EStateTreeEvent::TricksterLaunchPoisonGrenade);
		GetWorld()->GetTimerManager().ClearTimer(TryTriggeringPoisonGrenadeHandle);
	}
}

void ATrickster::AI_DestroyPoisonGrenadeTrigger() const
{
	if (AI_PoisonGrenadeTrigger)
	{
		AI_PoisonGrenadeTrigger->OnComponentBeginOverlap.RemoveAll(this);
		AI_PoisonGrenadeTrigger->DestroyComponent();
	}
}

void ATrickster::AI_DestroyVfxs()
{
	Super::AI_DestroyVfxs();
	if (!AI_CloneAbilityRunVfx) return;
	AI_CloneAbilityRunVfx->DeactivateImmediate();
	AI_CloneAbilityRunVfx->DestroyComponent();
	AI_CloneAbilityRunVfx = nullptr;
}

void ATrickster::AI_ReceiveEmpEffect(const float InEffectDuration)
{
	if (DelegateManager->RequestSpecialUpgradeAvailability.Execute(EUpgrade::TricksterAbility2, EUpgradeAttribute::Special1)) return;
	Super::AI_ReceiveEmpEffect(InEffectDuration);
}

void ATrickster::AI_OnPlayerEnterPoisonGrenadeTrigger(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsActionsDisabled() || !OtherActor->ActorHasTag("Player")) return;
	AI_TogglePoisonGrenadeTrigger(false);
	GetWorld()->GetTimerManager().SetTimer(TryTriggeringPoisonGrenadeHandle, this, &ATrickster::AI_TryTriggeringPoisonGrenadeLaunch, 0.1f, true);
}

void ATrickster::P_Setup()
{
	DelegateManager->OnWeaponAmmoChangeTwoParams.AddUObject(this, &AEnemy::P_UpdateAmmoHUD);
	Super::P_Setup();
	Weapon->GetHandsAnimInstance()->OnHandsMontageEnd.BindUObject(this, &ATrickster::SetIsActionInMotion);
}

void ATrickster::P_WeaponShoot() { Weapon->P_Shoot(EPoolObject::TricksterProjectile); }

float ATrickster::ReceiveMovementSpeedModification() const
{
	if (bIsCloneEnemyRunUpgradeTaken && !bIsHijacked && CurrentMoveMode == EMoveMode::Run) return AI_MovementSpeedMultiplierForCloneSpecialUpgrade;
	return 1.f + GetGameInstance()->GetSubsystem<UPlayerDataManager>()->GetTricksterDefaultMovementBoost();
}

void ATrickster::BeginPlay()
{
	CharacterName = ECharacterType::Trickster;
	Super::BeginPlay();
	bIsCloneEnemyRunUpgradeTaken = SaveSlotManager->GetRunSaveSlot()->IsSpecialUpgradeAvailable(EUpgrade::TricksterAbility3, EUpgradeAttribute::Special1);
	if (bIsCloneEnemyRunUpgradeTaken) AssetsManager->AttachVfx(EVfx::AI_TricksterCloneAbilityRun, [this]() -> bool { return IsAsyncAssetStillAllowedToBeUsed(); }, &AI_CloneAbilityRunVfx, GetMesh(), FName("DeathSocket"));
}

void ATrickster::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (NewController && NewController->IsPlayerController()) AI_DestroyPoisonGrenadeTrigger();
}

void ATrickster::InitiateDeath()
{
	AI_DestroyPoisonGrenadeTrigger();
	Super::InitiateDeath();
}

void ATrickster::SetMoveMode(const EMoveMode InMoveMode)
{
	Super::SetMoveMode(InMoveMode);
	if (!bIsCloneEnemyRunUpgradeTaken || !AI_CloneAbilityRunVfx) return;
	if (CurrentMoveMode == EMoveMode::Walk) AI_CloneAbilityRunVfx->DeactivateImmediate();
	else
	{
		PlaySound(ESound::TricksterCloneDash, true);
		AI_CloneAbilityRunVfx->Activate();
	}
}

void ATrickster::AllDisablingActionsFinished()
{
	Super::AllDisablingActionsFinished();
	if (!AI_PoisonGrenadeTrigger || AI_PoisonGrenadeTrigger->GetCollisionEnabled() != ECollisionEnabled::QueryOnly) return;
	AI_PoisonGrenadeTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AI_PoisonGrenadeTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}
