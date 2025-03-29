#include "Ranger.h"
#include "InputMappingContext.h"
#include "NiagaraComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"
#include "Components/ShapeComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/TriggerSphere.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAIController.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "PossessionGame/Destructibles/RangerShield.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Enums/EUpgradeAttribute.h"
#include "PossessionGame/UI/MainWidgets/PlayerHUD.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/UpgradeManager.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"
#include "PossessionGame/Weapons/Animation Instances/Hands/RangerHandsAnimInstance.h"
#include "PossessionGame/Weapons/Character Weapons/RangerWeapon.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

ARanger::ARanger()
{
	Sounds.Add(ESound::RangerSpawn, CreateDefaultSubobject<UAudioComponent>(TEXT("spawnSound")));
	Sounds[ESound::RangerSpawn]->SetAutoActivate(false);
	Sounds[ESound::RangerSpawn]->SetupAttachment(GetMesh());
	Sounds.Add(ESound::RangerDeath, CreateDefaultSubobject<UAudioComponent>(TEXT("deathSound")));
	Sounds[ESound::RangerDeath]->SetAutoActivate(false);
	Sounds[ESound::RangerDeath]->SetupAttachment(GetMesh());
	Sounds.Add(ESound::RangerGrenadeLaunch, CreateDefaultSubobject<UAudioComponent>(TEXT("grenadeLaunchSound")));
	Sounds[ESound::RangerGrenadeLaunch]->SetAutoActivate(false);
	Sounds[ESound::RangerGrenadeLaunch]->SetupAttachment(GetMesh());
	Sounds.Add(ESound::RangerGrenadeLaunchAim, CreateDefaultSubobject<UAudioComponent>(TEXT("grenadeLaunchAimSound")));
	Sounds[ESound::RangerGrenadeLaunchAim]->SetAutoActivate(false);
	Sounds[ESound::RangerGrenadeLaunchAim]->SetupAttachment(GetMesh());
}

void ARanger::LaunchGrenade()
{
	PlaySound(ESound::RangerGrenadeLaunch);
	Weapon->ShootProjectile(EPoolObject::RangerGrenadeLauncherProjectile, false);
	GrenadeLauncherMuzzleFlashLogic();
}

void ARanger::SpawnShield()
{
	const TObjectPtr<APoolObject> Shield = GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(EPoolObject::RangerShield, GetActorLocation() + GetActorForwardVector() * 100.f, GetActorRotation() + FRotator(0.f, -90.f, 0.f));
	Shield->Init(bIsPossessed);
	Cast<ARangerShield>(Shield)->SetProprietor(this);
}

void ARanger::SetCustomTimeDilation(float Dilation)
{
	if (FMath::IsNearlyEqual(Dilation, 5.f, 0.1f))
	{
		AssetsManager->AttachOrLoadVfx(EVfx::StopTimeAbility, nullptr, GetCamera(), NAME_None, false, FVector(20.f, 0.f, 0.f), FRotator::ZeroRotator, EAttachLocation::SnapToTarget, true);
	}
	else AssetsManager->AttachOrLoadVfx(EVfx::StopTimeAbilityReverse, nullptr, GetCamera(), NAME_None, false, FVector(20.f, 0.f, 0.f), FRotator::ZeroRotator, EAttachLocation::SnapToTarget, true);
	CustomTimeDilation = Dilation;
	GetWeapon()->CustomTimeDilation = Dilation;
}

void ARanger::AI_TriggerDeathVoiceLine() { AssetsManager->PlaySound2D(ESound::RangerOnPossession); }

void ARanger::AI_SetupWeapon()
{
	Weapon = GetWorld()->GetGameInstance()->GetSubsystem<UWeaponManager>()->GetWeapon(GetWorld(), EWeapon::RangerWeapon, GetMesh());
	Super::AI_SetupWeapon();
}

void ARanger::AI_PlayDeathSound() { PlaySound(ESound::RangerDeath); }

void ARanger::AI_ToggleGrenadeLaunchTrigger(const bool bState) const
{
	if (AIGrenadeLaunchTriggerSphere)
	{
		AIGrenadeLaunchTriggerSphere->GetCollisionComponent()->SetCollisionEnabled(bState ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
		AIGrenadeLaunchTriggerSphere->SetActorHiddenInGame(!bState);
	}
}

void ARanger::AI_PlaySpawnSound() { PlaySound(ESound::RangerSpawn); }

void ARanger::AI_OnPlayerEnterGrenadeLaunchTrigger(AActor* OverlappedActor, AActor* OtherActor)
{
	AI_ToggleGrenadeLaunchTrigger(false);
	UKismetSystemLibrary::K2_SetTimer(this, "TryTriggeringGrenadeLaunch", 0.1f, true);
}

void ARanger::P_Setup()
{
	UKismetSystemLibrary::K2_ClearTimer(this, "TryTriggeringGrenadeLaunch");
	DelegateManager->OnWeaponAmmoChangeTwoParams.AddUObject(this, &AEnemy::P_UpdateAmmoHUD);
	Super::P_Setup();
	Weapon->GetHandsAnimInstance()->OnHandsMontageEnd.BindUObject(this, &ARanger::SetActionInMotion);
}

void ARanger::P_SetupRightClickInputStartedState(TObjectPtr<UEnhancedInputComponent> RightClickInputComponent)
{
	RightClickInputComponent->BindAction(PlayerMappingContext->GetMapping(15).Action, ETriggerEvent::Started, this, &ARanger::P_ChangeAiming, true);
}

void ARanger::P_SetupRightClickInputCompletedState(TObjectPtr<UEnhancedInputComponent> RightClickInputComponent)
{
	RightClickInputComponent->BindAction(PlayerMappingContext->GetMapping(15).Action, ETriggerEvent::Completed, this, &ARanger::P_ChangeAiming, false);
}

void ARanger::P_ChangeAiming(bool bIsZooming) { Cast<ARangerWeapon>(Weapon)->P_ZoomToCamera(bIsZooming, MouseSensitivity); }

void ARanger::P_SetupColorsForUI() { P_HUD->SetColors(FLinearColor(0.58431f, 0.11373f, 0.f)); }

void ARanger::P_ApplyUpgradeOnSelection(const EUpgrade InUpgradeName)
{
	Super::P_ApplyUpgradeOnSelection(InUpgradeName);
	if (InUpgradeName == EUpgrade::RangerBaseStat)
	{
		const float HpValue = GetWorld()->GetSubsystem<UUpgradeManager>()->GetUpgradeAttributeValue(true, CharacterName, EUpgradeAttribute::FlatHealth);
		MaxHp += HpValue;
		CurrentHp += HpValue;
		if (P_HUD) P_HUD->UpdateProgressBar(EUIBar::Hp, CurrentHp, MaxHp);
	}
	else if (InUpgradeName == EUpgrade::RangerAbility1) Abilities[0]->ApplyUpgradeOnSelection(bIsPossessed);
}

void ARanger::P_Destroy()
{
	DelegateManager->ApplyUpgradeOnSelection.RemoveAll(this);
	DelegateManager->ApplyUpgradeOnSelection.RemoveAll(GetWeapon());
	Super::P_Destroy();
}

void ARanger::BeginPlay()
{
	CharacterName = ECharacterType::Ranger;
	Super::BeginPlay();
	SetupGrenadeLaunchTriggerSphere();
	AssetsManager->AttachOrLoadVfx(EVfx::RangerGrenadeLauncherMuzzle, &GrenadeLauncherMuzzleFlashComponent, Weapon->GetMesh(), FName("GrenadeLauncherMuzzleSocket"), true);
}

void ARanger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsPossessed) Cast<ARangerWeapon>(Weapon)->ZoomLogic(Camera, DeltaTime);
}

void ARanger::GrenadeLauncherMuzzleFlashLogic()
{
	if (GrenadeLauncherMuzzleFlashComponent)
	{
		if (GrenadeLauncherMuzzleFlashComponent->IsActive()) GrenadeLauncherMuzzleFlashComponent->Deactivate();
		GrenadeLauncherMuzzleFlashComponent->Activate();
	}
}

float ARanger::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float ReturnDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (!bIsActionInMotion && !bIsPossessed && CurrentHp <= MaxHp * 0.5f && CanAbilityBeExecuted(1)) AI_UseAbility(1);
	return ReturnDamage;
}

void ARanger::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	DestroyGrenadeLaunchTriggerSphere();
}

void ARanger::InitiateDeath()
{
	DestroyGrenadeLaunchTriggerSphere();
	Super::InitiateDeath();
}

void ARanger::SetupGrenadeLaunchTriggerSphere()
{
	AIGrenadeLaunchTriggerSphere = GetWorld()->SpawnActor<ATriggerSphere>(ATriggerSphere::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
	AIGrenadeLaunchTriggerSphere->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);
	AIGrenadeLaunchTriggerSphere->GetCollisionComponent()->SetCollisionProfileName("TriggerPlayer");
	AIGrenadeLaunchTriggerSphere->GetCollisionComponent()->CanCharacterStepUpOn = ECB_No;
	AIGrenadeLaunchTriggerSphere->GetCollisionComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Cast<USphereComponent>(AIGrenadeLaunchTriggerSphere->GetCollisionComponent())->SetSphereRadius(GrenadeLaunchTriggerRadiusInMetres * 100.f);
	AIGrenadeLaunchTriggerSphere->OnActorBeginOverlap.AddDynamic(this, &ARanger::AI_OnPlayerEnterGrenadeLaunchTrigger);
}

void ARanger::DestroyGrenadeLaunchTriggerSphere() const
{
	if (AIGrenadeLaunchTriggerSphere)
	{
		AIGrenadeLaunchTriggerSphere->OnActorBeginOverlap.RemoveAll(this);
		AIGrenadeLaunchTriggerSphere->Destroy();
	}
}

void ARanger::TryTriggeringGrenadeLaunch()
{
	if (!AI || AnimInstance->IsPlaying(EEnemyAnimState::Reload)) return;
	if (AI->GetStateTreeData().GetIsTargetInMySight())
	{
		AI->GetStateTreeData().TriggerEvent("StateTree.Ranger.LaunchGrenade");
		UKismetSystemLibrary::K2_ClearTimer(this, "TryTriggeringGrenadeLaunch");
	}
}
