#include "Samurai.h"
#include "InputMappingContext.h"
#include "PossessionGame/AI/Enemies/Animation Instances/SamuraiAnimationInstance.h"
#include "PossessionGame/Destructibles/Destructible.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EPlayerInputState.h"
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
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnWeaponAmmoChangeOneParam.AddUObject(this, &AEnemy::P_UpdateAmmoHUD);
	Super::P_Setup();
	SamuraiHat->HandleDeactivation();
}

void ASamurai::P_SetupLeftClickInputStartedState(TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent)
{
	Super::P_SetupLeftClickInputStartedState(LeftClickInputComponent);
	LeftClickInputComponent->BindAction(PlayerMappingContext->GetMapping(8).Action, ETriggerEvent::Started, this, &ASamurai::P_SetBowStretchingState, EPlayerInputState::LeftClickStart);
}

void ASamurai::P_SetupLeftClickInputCompletedState(TObjectPtr<UEnhancedInputComponent> LeftClickInputComponent)
{
	Super::P_SetupLeftClickInputCompletedState(LeftClickInputComponent);
	LeftClickInputComponent->BindAction(PlayerMappingContext->GetMapping(8).Action, ETriggerEvent::Completed, this, &ASamurai::P_SetBowStretchingState, EPlayerInputState::LeftClickRelease);
}

void ASamurai::P_SetupRightClickInputStartedState(TObjectPtr<UEnhancedInputComponent> RightClickInputComponent)
{
	RightClickInputComponent->BindAction(PlayerMappingContext->GetMapping(15).Action, ETriggerEvent::Started, this, &ASamurai::P_SetBowStretchingState, EPlayerInputState::RightClickStart);
}

void ASamurai::P_SetBowStretchingState(EPlayerInputState bState) { Cast<ASamuraiWeapon>(Weapon)->P_SetStretchingState(bState); }

void ASamurai::P_SetupColorsForUI() { P_HUD->SetColors(FLinearColor(0.2745f, 0.f, 0.5843f)); }

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
