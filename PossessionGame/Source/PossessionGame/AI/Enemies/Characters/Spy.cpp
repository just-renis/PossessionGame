#include "Spy.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "PossessionGame/AI/Enemies/Controllers/SpyAIController.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"
#include "PossessionGame/Weapons/Character Weapons/SpyWeapon.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

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
}

void ASpy::P_SetupLeftClickInputStartedState()
{
	EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(8).Action, ETriggerEvent::Started, this, &ASpy::P_Shoot);
}

void ASpy::P_WeaponShoot() { Weapon->P_Shoot(EPoolObject::SpyProjectile); }

void ASpy::BeginPlay()
{
	CharacterName = ECharacterType::Spy;
	Super::BeginPlay();
}
