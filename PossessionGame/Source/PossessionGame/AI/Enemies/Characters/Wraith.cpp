#include "Wraith.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EWeapon.h"
#include "PossessionGame/Projectiles/Throwables/WraithMolotov.h"
#include "PossessionGame/Projectiles/Throwables/WraithTrap.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void AWraith::P_InitiateThrowMolotov()
{
	const TObjectPtr<AWraithMolotov> Molotov = Cast<AWraithMolotov>(GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectWithCapFromPool(EPoolObject::WraithMolotov, Weapon->GetHandsSocketLocation(FName("ThrowSocket")), Weapon->GetHandsSocketRotation(FName("ThrowSocket"))));
	if (!Molotov) return;
	Molotov->Initialize(this);
	Molotov->Init();
	//AssetsManager->PlaySound2D(ESound::RangerThrowMine);
	Molotov->Throw();
	SetIsActionInMotion(false); // WHEN ANIMATION EXISTS REMOVE THIS SHIT.
}

void AWraith::P_InitiateThrowTrap()
{
	const TObjectPtr<AWraithTrap> Trap = Cast<AWraithTrap>(GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectWithCapFromPool(EPoolObject::WraithTrap, Weapon->GetHandsSocketLocation(FName("ThrowSocket")), Weapon->GetHandsSocketRotation(FName("ThrowSocket"))));
	if (!Trap) return;
	Trap->Initialize(this);
	Trap->Init();
	//AssetsManager->PlaySound2D(ESound::RangerThrowMine);
	Trap->Throw();
	SetIsActionInMotion(false); // WHEN ANIMATION EXISTS REMOVE THIS SHIT.
}

void AWraith::AI_SetupWeapon()
{
	Weapon = GetWorld()->GetGameInstance()->GetSubsystem<UWeaponManager>()->GetWeapon(GetWorld(), EWeapon::WraithWeapon, GetMesh());
	Super::AI_SetupWeapon();
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
