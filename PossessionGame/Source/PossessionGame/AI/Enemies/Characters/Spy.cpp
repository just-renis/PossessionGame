#include "Spy.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "NiagaraComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/AI/Enemies/Controllers/SpyAIController.h"
#include "PossessionGame/Enums/ECameraShake.h"
#include "PossessionGame/Enums/EPostProcessMaterial.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"
#include "PossessionGame/Weapons/Character Weapons/SpyWeapon.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PostProcessManager.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"

void ASpy::StartTeleportProcess()
{
	bIsInvincible = true;
	bIsSecondAbilityActive = true;
	if (!TeleportLinesVfx) AssetsManager->AttachVfx(EVfx::SpyTeleportLines, [this]() -> bool { return GetIsAlive() && bIsSecondAbilityActive && bIsHijacked; }, &TeleportLinesVfx, Camera);
	else TeleportLinesVfx->Activate(true);
	SetTeleportMaterialValue(1.f);
}

void ASpy::FinishTeleportProcess()
{
	P_MakeCameraShake(ECameraShake::SpyTeleportKillCameraShake);
	Camera->SetWorldLocation(GetActorLocation() + FVector::UpVector * 60.f);
	bIsInvincible = false;
	bIsSecondAbilityActive = false;
	if (TeleportLinesVfx) TeleportLinesVfx->DeactivateImmediate();
	SetTeleportMaterialValue(0.f);
}

void ASpy::AI_SetupWeapon()
{
	Weapon = GetWorld()->GetGameInstance()->GetSubsystem<UWeaponManager>()->GetWeapon(GetWorld(), EWeapon::SpyWeapon, GetMesh());
	Super::AI_SetupWeapon();
}

void ASpy::AI_SetController() { AI = Cast<ASpyAIController>(GetController()); }

void ASpy::P_Setup()
{
	DelegateManager->OnWeaponAmmoChangeTwoParams.AddUObject(this, &AEnemy::P_UpdateAmmoHUD);
	Super::P_Setup();
	Weapon->GetHandsAnimInstance()->OnHandsMontageEnd.BindUObject(this, &ASpy::SetIsActionInMotion);
}

void ASpy::P_SetupLeftClickInputStartedState()
{
	EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(8).Action, ETriggerEvent::Started, this, &ASpy::P_Shoot);
}

void ASpy::P_WeaponShoot() { Weapon->P_Shoot(EPoolObject::SpyProjectile); }

void ASpy::P_MakeCameraShake(const ECameraShake InCameraShakeName)
{
	if (InCameraShakeName == ECameraShake::SpyTeleportKillCameraShake) UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraShake(TeleportKillCameraShake);
	else Super::P_MakeCameraShake(InCameraShakeName);
}

void ASpy::BeginPlay()
{
	CharacterName = ECharacterType::Spy;
	Super::BeginPlay();
}

void ASpy::SetTeleportMaterialValue(const float InValue)
{
	if (!MID_Teleport) MID_Teleport = UMaterialInstanceDynamic::Create(MI_Teleport, this);
	if (InValue > 0.1f)
	{
		MID_Teleport->SetScalarParameterValue(FName("Density"), 2.3f);
		MID_Teleport->SetScalarParameterValue(FName("Intensity"), 1.8f);
		MID_Teleport->SetScalarParameterValue(FName("Radius"), 0.2f);
	}
	else
	{
		MID_Teleport->SetScalarParameterValue(FName("Density"), 0.f);
		MID_Teleport->SetScalarParameterValue(FName("Intensity"), 0.f);
		MID_Teleport->SetScalarParameterValue(FName("Radius"), 0.f);
	}
	GetWorld()->GetSubsystem<UPostProcessManager>()->AddOrUpdateDynamicMaterial(EPostProcessMaterial::SpyTeleportEffect, MID_Teleport, InValue);
}
