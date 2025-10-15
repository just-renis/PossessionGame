#include "Breacher.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EWeapon.h"
#include "PossessionGame/Projectiles/Throwables/BreacherBarrel.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void ABreacher::AI_SetupWeapon()
{
	Weapon = GetWorld()->GetGameInstance()->GetSubsystem<UWeaponManager>()->GetWeapon(GetWorld(), EWeapon::BreacherWeapon, GetMesh());
	Super::AI_SetupWeapon();
}

void ABreacher::P_Setup()
{
	DelegateManager->OnWeaponAmmoChangeTwoParams.AddUObject(this, &AEnemy::P_UpdateAmmoHUD);
	Super::P_Setup();
	//P_ChangeRangerVariationMuzzle();
	Weapon->GetHandsAnimInstance()->OnHandsMontageEnd.BindUObject(this, &ABreacher::SetIsActionInMotion);
}

void ABreacher::P_ThrowBarrel()
{
	const TObjectPtr<ABreacherBarrel> Barrel = Cast<ABreacherBarrel>(GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(EPoolObject::BreacherBarrel, Weapon->GetHandsSocketLocation(FName("BarrelSocket")), Weapon->GetHandsSocketRotation(FName("BarrelSocket"))));
	if (!Barrel) return;
	Barrel->Initialize(this);
	Barrel->Init(true);
	//AssetsManager->PlaySound2D(ESound::RangerThrowMine);
	Barrel->Throw();
}

void ABreacher::BeginPlay()
{
	CharacterName = ECharacterType::Breacher;
	Super::BeginPlay();
}
