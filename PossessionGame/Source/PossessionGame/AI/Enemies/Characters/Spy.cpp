#include "Spy.h"

#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "PossessionGame/Abilities/Management/Ability.h"
#include "PossessionGame/AI/Enemies/Controllers/SpyAIController.h"
#include "PossessionGame/UI/MainWidgets/PlayerHUD.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"
#include "PossessionGame/Weapons/Character Weapons/SpyWeapon.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

void ASpy::SetConceal(const bool bState)
{
	Cast<ASpyWeapon>(Weapon)->SetConceal(bState);
	GetMesh()->SetScalarParameterValueOnMaterials("Opacity", bState ? 0.2f : 1.f);
}

void ASpy::AI_SetupWeapon()
{
	Weapon = GetWorld()->GetGameInstance()->GetSubsystem<UWeaponManager>()->GetWeapon(GetWorld(), EWeapon::SpyWeapon, GetMesh());
	Super::AI_SetupWeapon();
}

void ASpy::AI_ShootLogic()
{
	StopConcealingAbility(false);
	Super::AI_ShootLogic();
}

void ASpy::AI_SetController() { AI = Cast<ASpyAIController>(GetController()); }

void ASpy::P_Setup()
{
	DelegateManager->OnWeaponAmmoChangeTwoParams.AddUObject(this, &AEnemy::P_UpdateAmmoHUD);
	Super::P_Setup();
}

void ASpy::P_Shoot()
{
	StopConcealingAbility();
	Super::P_Shoot();
}

void ASpy::P_SetupLeftClickInputStartedState(TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent)
{
	Super::P_SetupLeftClickInputStartedState(LeftClickInputComponent);
	LeftClickInputComponent->BindAction(PlayerMappingContext->GetMapping(8).Action, ETriggerEvent::Started, this, &ASpy::P_Shoot);
}

void ASpy::P_SetupColorsForUI() { P_HUD->SetColors(FLinearColor(0.20392f, 0.58431f, 0.31373f)); }

void ASpy::P_WeaponShoot() { Weapon->P_Shoot(EPoolObject::SpyProjectile); }

void ASpy::BeginPlay()
{
	CharacterName = ECharacterType::Spy;
	Super::BeginPlay();
}

void ASpy::StopConcealingAbility(const bool bIsPlayer)
{
	if (Abilities.IsEmpty()) return;
	bIsPlayer ? Abilities[0]->P_StopAbility() : Abilities[0]->AI_StopAbility();
}
