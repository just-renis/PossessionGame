#include "Tank.h"
#include "EnhancedPlayerInput.h"
#include "InputMappingContext.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/UI/PlayerHUD.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"
#include "PossessionGame/Weapons/Character Weapons/TankWeapon.h"
#include "PossessionGame/Weapons/Management/Weapon.h"
#include "PossessionGame/Weapons/Management/WeaponAnimInstance.h"

void ATank::P_Shoot(bool bFirstWeapon)
{
	if (Weapon->P_CanShoot()) Weapon->P_Shoot();
}

void ATank::AI_SetupWeapon()
{
	Weapon = GetWorld()->GetGameInstance()->GetSubsystem<UWeaponManager>()->GetWeapon(GetWorld(), EWeapon::TankWeapon, GetMesh());
	Super::AI_SetupWeapon();
}

bool ATank::P_GetIsLeftOrRightInputNotPressed()
{
	return !P_InputManager->IsPressed(FKey("LeftMouseButton")) && !P_InputManager->IsPressed(FKey("RightMouseButton"));
}

void ATank::P_Setup()
{
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnWeaponAmmoChangeOneParam.AddUObject(this, &AEnemy::P_UpdateAmmoHUD);
	Super::P_Setup();
}

void ATank::P_SetupLeftClickInputStartedState(TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent)
{
	Super::P_SetupLeftClickInputStartedState(LeftClickInputComponent);
	LeftClickInputComponent->BindAction(PlayerMappingContext->GetMapping(8).Action, ETriggerEvent::Started, this, &ATank::P_ToggleChargeMinigun, true);
}

void ATank::P_SetupLeftClickInputCompletedState(TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent)
{
	Super::P_SetupLeftClickInputCompletedState(LeftClickInputComponent);
	LeftClickInputComponent->BindAction(PlayerMappingContext->GetMapping(8).Action, ETriggerEvent::Completed, this, &ATank::P_ToggleChargeMinigun, false);
}

void ATank::P_SetupRightClickInputStartedState(TObjectPtr<UEnhancedInputComponent> RightClickInputComponent)
{
	RightClickInputComponent->BindAction(PlayerMappingContext->GetMapping(15).Action, ETriggerEvent::Started, this, &ATank::P_ToggleChargeMinigun, true);
}

void ATank::P_SetupRightClickInputCompletedState(TObjectPtr<UEnhancedInputComponent> RightClickInputComponent)
{
	RightClickInputComponent->BindAction(PlayerMappingContext->GetMapping(15).Action, ETriggerEvent::Completed, this, &ATank::P_ToggleChargeMinigun, false);
}

void ATank::P_DisableEnemy()
{
	Super::P_DisableEnemy();
	Weapon->AttachToSocket(GetMesh());
}

void ATank::P_ToggleChargeMinigun(bool bState)
{
	if (Weapon->GetCurrentAmmoAmount() <= 0) return;
	if (bState)
	{
		if (!Weapon->GetAnimInstance()->IsPlaying(EWeaponAnimState::ChargeUp) && !Weapon->GetAnimInstance()->IsPlaying(EWeaponAnimState::Shoot)) Weapon->GetAnimInstance()->PlayMontage(EWeaponAnimState::ChargeUp);
	}
	else P_StopChargingMinigun();
	if (P_GetIsLeftOrRightInputNotPressed() || bState) SetInitialWalkSpeed(!bState);
}

void ATank::P_StopChargingMinigun()
{
	if (Weapon->GetAnimInstance()->IsPlaying(EWeaponAnimState::ChargeUp)) Weapon->GetAnimInstance()->StopMontage(EWeaponAnimState::ChargeUp);
	else if (P_GetIsLeftOrRightInputNotPressed()) Weapon->GetAnimInstance()->StopMontage(EWeaponAnimState::Shoot, 0.1f);
}

void ATank::P_SetupColorsForUI() { P_HUD->SetColors(FLinearColor(0.57647f, 0.58431f, 0.56078f)); }

void ATank::P_StopMinigun()
{
	if (Weapon->GetAnimInstance()->IsPlaying(EWeaponAnimState::ChargeUp)) Weapon->GetAnimInstance()->StopMontage(EWeaponAnimState::ChargeUp);
	else Weapon->GetAnimInstance()->StopMontage(EWeaponAnimState::Shoot, 0.1f);
	SetInitialWalkSpeed(true);
}

void ATank::BeginPlay()
{
	CharacterName = ECharacterType::Tank;
	bCanRun = false;
	bCanReload = false;
	Super::BeginPlay();
	Cast<ATankWeapon>(Weapon)->OnAmmoEnded.BindUObject(this, &ATank::P_StopMinigun);
}
