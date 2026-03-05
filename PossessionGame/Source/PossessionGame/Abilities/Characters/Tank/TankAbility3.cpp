#include "TankAbility3.h"
#include "PossessionGame/AI/Enemies/Characters/Tank.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void UTankAbility3::P_Execute()
{
	Proprietor->SetIsActionInMotion(true);
	Proprietor->GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::TankSlamJump);
	const FVector LaunchVelocity = Proprietor->GetActorForwardVector() * ForwardImpulseStrength + FVector::UpVector * UpImpulseStrength;
	Proprietor->LaunchCharacter(LaunchVelocity, true, true);
	Proprietor->GetWeapon()->GetHandsAnimInstance()->PlayMontage(EHandsAnimState::JumpSecondary);
	Super::P_Execute();
}

bool UTankAbility3::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void UTankAbility3::SetProprietor(const TObjectPtr<AEnemy>& InAbilityProprietor) { Proprietor = Cast<ATank>(InAbilityProprietor); }
