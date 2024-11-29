#include "Ranger.h"
#include "InputMappingContext.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "PossessionGame/Data Assets/Primary Data Assets/PA_VfxContainer.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EPoolObject.h"
#include "PossessionGame/UI/PlayerHUD.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"
#include "PossessionGame/Weapons/Animation Instances/Hands/RangerHandsAnimInstance.h"
#include "PossessionGame/Weapons/Character Weapons/RangerWeapon.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void ARanger::LaunchGrenade()
{
	Weapon->ShootProjectile(EPoolObject::RangerGrenadeLauncherProjectile);
	GrenadeLauncherMuzzleFlashLogic();
}

void ARanger::SpawnShield()
{
	GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(bIsPossessed, EPoolObject::RangerShield, GetActorLocation() + GetActorForwardVector() * 100.f, GetActorRotation());
}

void ARanger::SetCustomTimeDilation(float Dilation)
{
	if (FMath::IsNearlyEqual(Dilation, 5.f, 0.1f))
	{
		if (AssetsManager->GetVfxData(EVfx::StopTimeAbility)->GetVfxEffect().IsValid()) SpawnStopTimeAbilityVfx(EVfx::StopTimeAbility);
		else AssetsManager->GetVfxData(EVfx::StopTimeAbility)->LoadVfx(GetWorld(), [this] { SpawnStopTimeAbilityVfx(EVfx::StopTimeAbility); });
	}
	else if (AssetsManager->GetVfxData(EVfx::StopTimeAbilityReverse)->GetVfxEffect().IsValid()) SpawnStopTimeAbilityVfx(EVfx::StopTimeAbilityReverse);
	else AssetsManager->GetVfxData(EVfx::StopTimeAbilityReverse)->LoadVfx(GetWorld(), [this] { SpawnStopTimeAbilityVfx(EVfx::StopTimeAbilityReverse); });
	CustomTimeDilation = Dilation;
	GetWeapon()->CustomTimeDilation = Dilation;
}

void ARanger::AI_TriggerDeathVoiceLine() { AssetsManager->PlaySound2D(ESound::AIRangerDeathVoiceLine); }

void ARanger::AI_SetupWeapon()
{
	Weapon = GetWorld()->GetGameInstance()->GetSubsystem<UWeaponManager>()->GetWeapon(GetWorld(), EWeapon::RangerWeapon, GetMesh());
	Super::AI_SetupWeapon();
}

void ARanger::P_Setup()
{
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnWeaponAmmoChangeTwoParams.AddUObject(this, &AEnemy::P_UpdateAmmoHUD);
	Super::P_Setup();
	Cast<URangerHandsAnimInstance>(Weapon->GetHandsAnimInstance())->OnGrenadeLaunchEnd.BindUObject(this, &ARanger::SetCanShootAndReload);
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

void ARanger::BeginPlay()
{
	CharacterName = ECharacterType::Ranger;
	Super::BeginPlay();
	AttachOrLoadGrenadeLauncherMuzzle();
}

void ARanger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsPossessed) Cast<ARangerWeapon>(Weapon)->ZoomLogic(Camera, DeltaTime);
}

void ARanger::SpawnStopTimeAbilityVfx(EVfx VfxName)
{
	UNiagaraFunctionLibrary::SpawnSystemAttached(AssetsManager->GetVfxData(VfxName)->GetVfxEffect().Get(), GetCamera(), NAME_None, FVector(20.f, 0.f, 0.f), FRotator::ZeroRotator, EAttachLocation::SnapToTargetIncludingScale, true);
}

void ARanger::AttachOrLoadGrenadeLauncherMuzzle()
{
	if (AssetsManager->GetVfxData(EVfx::RangerGrenadeLauncherMuzzle)->GetVfxEffect().IsValid()) AttachGrenadeLauncherMuzzle();
	else AssetsManager->GetVfxData(EVfx::RangerGrenadeLauncherMuzzle)->LoadVfx(GetWorld(), [this] { AttachGrenadeLauncherMuzzle(); });
}

void ARanger::AttachGrenadeLauncherMuzzle()
{
	GrenadeLauncherMuzzleFlashComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(AssetsManager->GetVfxData(EVfx::RangerGrenadeLauncherMuzzle)->GetVfxEffect().Get(), Weapon->GetMesh(), FName("GrenadeLauncherMuzzleSocket"),
	FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false);
	GrenadeLauncherMuzzleFlashComponent->Deactivate();
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
	if (CurrentHp <= MaxHp * 0.5f) if (!bIsPossessed && CanAbilityBeExecuted(1)) AI_UseAbility(1);
	return ReturnDamage;
}
