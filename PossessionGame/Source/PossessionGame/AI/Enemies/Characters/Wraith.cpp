#include "Wraith.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "PossessionGame/AI/Enemies/Controllers/WraithAIController.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EWeapon.h"
#include "PossessionGame/Projectiles/Throwables/WraithMeteorTrigger.h"
#include "PossessionGame/Projectiles/Throwables/WraithMolotov.h"
#include "PossessionGame/Projectiles/Throwables/WraithTrap.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/OvertimeDamagers/OvertimeDamager.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void AWraith::P_InitiateThrowMolotov()
{
	const TObjectPtr<AWraithMolotov> Molotov = Cast<AWraithMolotov>(GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(EPoolObject::WraithMolotov, Weapon->GetHandsSocketLocation(FName("ThrowSocket")), Weapon->GetHandsSocketRotation(FName("ThrowSocket"))));
	if (!Molotov) return;
	Molotov->InitWithProprietor(this);
	AssetsManager->PlaySound2D(ESound::WraithThrow);
	Molotov->Throw();
}

void AWraith::P_InitiateThrowTrap()
{
	const TObjectPtr<AWraithTrap> Trap = Cast<AWraithTrap>(GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(EPoolObject::WraithTrap, Weapon->GetHandsSocketLocation(FName("ThrowSocket")), Weapon->GetHandsSocketRotation(FName("ThrowSocket"))));
	if (!Trap) return;
	Trap->InitWithProprietor(this);
	AssetsManager->PlaySound2D(ESound::WraithThrow);
	Trap->Throw();
}

void AWraith::P_InitiateThrowMeteorShower()
{
	const TObjectPtr<AWraithMeteorTrigger> MeteorTrigger = Cast<AWraithMeteorTrigger>(GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(EPoolObject::WraithMeteorTrigger, Weapon->GetHandsSocketLocation(FName("ThrowSocket")), Weapon->GetHandsSocketRotation(FName("ThrowSocket"))));
	if (!MeteorTrigger) return;
	MeteorTrigger->InitWithProprietor(this);
	AssetsManager->PlaySound2D(ESound::WraithThrow);
	MeteorTrigger->Throw();
}

AWraith::AWraith()
{
	SpawnSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("SpawnSfx"));
	SpawnSfx->SetAutoActivate(false);
	SpawnSfx->SetupAttachment(GetMesh());
	DeathSfx = CreateDefaultSubobject<UAudioComponent>(TEXT("DeathSfx"));
	DeathSfx->SetAutoActivate(false);
	DeathSfx->SetupAttachment(GetMesh());
	Sounds.Add(ESound::WraithSpawn, SpawnSfx);
	Sounds.Add(ESound::WraithDeath, DeathSfx);
}

void AWraith::AI_SetupWeapon()
{
	Weapon = GetWorld()->GetGameInstance()->GetSubsystem<UWeaponManager>()->GetWeapon(GetWorld(), EWeapon::WraithWeapon, GetMesh());
	Super::AI_SetupWeapon();
}

void AWraith::AI_PlaySpawnSound() { PlaySound(ESound::WraithSpawn, true); }

void AWraith::AI_PlayDeathSound()
{
	Super::AI_PlayDeathSound();
	PlaySound(ESound::WraithDeath);
}

void AWraith::AI_TriggerDeathVoiceLine() { AssetsManager->PlaySound2D(ESound::WraithOnHijack); }

void AWraith::AI_PlayDeathElectrification()
{
	AssetsManager->AttachVfx(EVfx::WraithDeathElectrification, nullptr, nullptr, GetMesh(), "DeathSocket");
}

void AWraith::AI_EnableTriggeringMeteorShower()
{
	if (!IsActionsDisabled()) GetWorldTimerManager().SetTimer(AI_TriggerMeteorShowerHandle, this, &AWraith::AI_TryTriggeringMeteorShower, 1.f, true);
	bIsMeteorShowerTriggerInitiated = true;
}

void AWraith::AI_InitiateMeteorShower()
{
	if (!MeteorChargeBallVfx) GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->AttachVfx(EVfx::EnemyWraithMeteorChargeBall, [this]() -> bool { return IsAsyncAssetStillAllowedToBeUsed(); }, &MeteorChargeBallVfx, GetMesh(), FName("MeteorChargeBallSocket"));
	else MeteorChargeBallVfx->Activate(true);
	FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 500.f;
	FHitResult GroundHit;
	if (GetWorld()->LineTraceSingleByChannel(GroundHit, SpawnLocation, SpawnLocation + FVector::DownVector * 2000.f, ECC_Visibility)) SpawnLocation = GroundHit.ImpactPoint;
	const TObjectPtr<AOvertimeDamager> MeteorShowerArea = Cast<AOvertimeDamager>(GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(EPoolObject::WraithMeteorShowerArea, SpawnLocation + FVector::UpVector));
	MeteorShowerArea->InitWithProprietor(this);
	MeteorShowerArea->Setup(false, AI_MeteorShowerData, EDamageReason::None);
}

void AWraith::AI_FinishMeteorShower()
{
	if (MeteorChargeBallVfx) MeteorChargeBallVfx->DeactivateImmediate();
	GetAnimInstance()->SetIsControlRigEnabled(true);
	SetIsActionInMotion(false);
	(void)Cast<AWraithAIController>(GetAI())->OnMeteorShowerFinished.ExecuteIfBound();
	AI_StopAbility(2);
}

void AWraith::AI_TryTriggeringMeteorShower()
{
	if (!CanAbilityBeExecuted(2) || GetIsActionInMotion() || GetIsAbilityInAction() || !AI || !AI->GetStateTreeData().GetStateTreeRef() || !AI->GetStateTreeData().GetIsTargetInMySight()) return;
	AI->GetStateTreeData().TriggerEvent(EStateTreeEvent::WraithMeteorShower);
	bIsMeteorShowerTriggerInitiated = false;
}

void AWraith::P_Setup()
{
	DelegateManager->OnWeaponAmmoChangeOneParam.AddUObject(this, &AEnemy::P_UpdateAmmoHUD);
	Super::P_Setup();
	Weapon->GetHandsAnimInstance()->OnHandsMontageEnd.BindUObject(this, &AWraith::SetIsActionInMotion);
}

void AWraith::P_WeaponShoot() { Weapon->P_Shoot(EPoolObject::WraithProjectile); }

void AWraith::BeginPlay()
{
	CharacterName = ECharacterType::Wraith;
	Super::BeginPlay();
}

void AWraith::AllDisablingActionsFinished()
{
	Super::AllDisablingActionsFinished();
	if (bIsMeteorShowerTriggerInitiated) AI_EnableTriggeringMeteorShower();
}

void AWraith::ClearTimers()
{
	Super::ClearTimers();
	if (MeteorChargeBallVfx) MeteorChargeBallVfx->DeactivateImmediate();
	GetWorldTimerManager().ClearTimer(AI_TriggerMeteorShowerHandle);
	bIsMeteorShowerTriggerInitiated = false;
}
