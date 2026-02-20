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

void ASpy::SetXrayState(const bool bState)
{
	bXrayEnabled = bState;
	if (!MID_Xray) MID_Xray = UMaterialInstanceDynamic::Create(MI_Xray, this);
	if (!MID_DarkXray) MID_DarkXray = UMaterialInstanceDynamic::Create(MID_DarkXray, this);
	if (bState)
	{
		GetWorld()->GetSubsystem<UPostProcessManager>()->AddOrUpdateDynamicMaterial(EPostProcessMaterial::SpyXrayEffect, MID_Xray, 1.f);
		GetWorld()->GetSubsystem<UPostProcessManager>()->AddOrUpdateDynamicMaterial(EPostProcessMaterial::SpyDarkXrayEffect, MID_DarkXray, 1.f);
		GetWorldTimerManager().SetTimer(ProcessXrayMaterialsHandle, this, &ASpy::ProcessXrayMaterialsActivation, 0.025f, true);
	}
	else GetWorldTimerManager().SetTimer(ProcessXrayMaterialsHandle, this, &ASpy::ProcessXrayMaterialsDeactivation, 0.025f, true);
}

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
	bIsSecondAbilityActive = false;
	P_MakeCameraShake(ECameraShake::SpyTeleportKillCameraShake);
	Camera->SetWorldLocation(GetActorLocation() + FVector::UpVector * 60.f);
	bIsInvincible = false;
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

void ASpy::ProcessXrayMaterialsActivation()
{
	if (!MID_Xray || !MID_DarkXray) return;
	XrayColorRadius -= 0.16f;
	XrayRadialRadius -= 0.03f;
	XrayDarkness -= 0.1f;
	UE_LOG(LogTemp, Error, TEXT("ProcessXrayMaterialsActivation %f"), XrayDarkness);
	MID_Xray->SetScalarParameterValue(FName("ColorRadius"), XrayColorRadius);
	MID_Xray->SetScalarParameterValue(FName("RadialRadius"), XrayRadialRadius);
	MID_DarkXray->SetScalarParameterValue(FName("Darkness"), XrayDarkness);
	if (++XrayCountLoops >= 10)
	{
		GetWorldTimerManager().ClearTimer(ProcessXrayMaterialsHandle);
		XrayCountLoops = 0;
	}
}

void ASpy::ProcessXrayMaterialsDeactivation()
{
	if (!MID_Xray || !MID_DarkXray) return;
	XrayColorRadius += 0.16f;
	XrayRadialRadius += 0.03f;
	XrayDarkness += 0.1f;
	UE_LOG(LogTemp, Error, TEXT("ProcessXrayMaterialsDeactivation %f"), XrayDarkness);
	MID_Xray->SetScalarParameterValue(FName("ColorRadius"), XrayColorRadius);
	MID_Xray->SetScalarParameterValue(FName("RadialRadius"), XrayRadialRadius);
	MID_DarkXray->SetScalarParameterValue(FName("Darkness"), XrayDarkness);
	if (++XrayCountLoops >= 10)
	{
		GetWorldTimerManager().ClearTimer(ProcessXrayMaterialsHandle);
		XrayCountLoops = 0;
		GetWorld()->GetSubsystem<UPostProcessManager>()->AddOrUpdateDynamicMaterial(EPostProcessMaterial::SpyXrayEffect, MID_Xray, 0.f);
		GetWorld()->GetSubsystem<UPostProcessManager>()->AddOrUpdateDynamicMaterial(EPostProcessMaterial::SpyDarkXrayEffect, MID_DarkXray, 0.f);
	}
}
