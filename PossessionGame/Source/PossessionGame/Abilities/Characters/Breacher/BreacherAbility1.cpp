#include "BreacherAbility1.h"
#include "PossessionGame/AI/Enemies/Characters/Breacher.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Weapons/Character Weapons/GuardianWeapon.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"

void UBreacherAbility1::P_Execute()
{
	Proprietor->SetIsActionInMotion(true);
	Proprietor->GetWeapon()->GetHandsAnimInstance()->PlayMontage(EHandsAnimState::BreacherThrow);
	Proprietor->P_ThrowBarrel();
//	GetWorld()->GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::GuardianOverChargeStart);
	Super::P_Execute();
}

bool UBreacherAbility1::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void UBreacherAbility1::SetProprietor(TObjectPtr<AEnemy> AbilityProprietor) { Proprietor = Cast<ABreacher>(AbilityProprietor); }
