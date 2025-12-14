#include "Samurai.h"
#include "PossessionGame/AI/Enemies/Animation Instances/SamuraiAnimationInstance.h"
#include "PossessionGame/Destructibles/Destructible.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EWeapon.h"
#include "PossessionGame/UI/MainWidgets/PlayerHUD.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"
#include "PossessionGame/Weapons/Character Weapons/SamuraiWeapon.h"
#include "PossessionGame/Weapons/Management/WeaponAnimInstance.h"

void ASamurai::AI_Shoot() { AI_PlayShootMontage(); }

void ASamurai::AI_SetupWeapon()
{
	Weapon = GetWorld()->GetGameInstance()->GetSubsystem<UWeaponManager>()->GetWeapon(GetWorld(), EWeapon::SamuraiWeapon, GetMesh());
	Super::AI_SetupWeapon();
}

void ASamurai::P_Setup()
{
	DelegateManager->OnWeaponAmmoChangeOneParam.AddUObject(this, &AEnemy::P_UpdateAmmoHUD);
	Super::P_Setup();
	SamuraiHat->HandleDeactivation();
}

/*void ASamurai::P_SetupRightClickInputStartedState(TObjectPtr<UEnhancedInputComponent> RightClickInputComponent)
{
	RightClickInputComponent->BindAction(PlayerMappingContext->GetMapping(14).Action, ETriggerEvent::Started, this, &ASamurai::P_SetBowStretchingState, EPlayerInputState::RightClickStart);
}*/

void ASamurai::P_SetupColorsForUI() { P_HUD->SetColors(FLinearColor(0.2745f, 0.f, 0.5843f)); }

void ASamurai::P_WeaponShoot() { Weapon->P_Shoot(EPoolObject::SamuraiProjectile); }

void ASamurai::BeginPlay()
{
	CharacterName = ECharacterType::Samurai;
	Super::BeginPlay();
	SetupSamuraiHat();
}

void ASamurai::SetupSamuraiHat()
{
	SamuraiHat = GetWorld()->SpawnActor<ADestructible>(SamuraiHatClass);
	AttachDestructible(SamuraiHat, "HatSocket");
}

void ASamurai::HandleDeactivation()
{
	if (!SamuraiHat->GetIsDeactivated()) SamuraiHat->HandleDeactivation();
	Super::HandleDeactivation();
}

void ASamurai::SetAnimInstance() { AnimInstance = Cast<USamuraiAnimationInstance>(GetMesh()->GetAnimInstance()); }

void ASamurai::AI_PlayShootMontage()
{
	if (AnimInstance->IsMontageExisting(EEnemyAnimState::Shoot) && !AnimInstance->IsPlaying(EEnemyAnimState::Shoot))
	{
		AnimInstance->PlayMontage(EEnemyAnimState::Shoot);
		Weapon->GetAnimInstance()->PlayMontage(EWeaponAnimState::AIShoot);
	}
}
