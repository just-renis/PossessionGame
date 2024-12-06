#include "RangerAnimationInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/AI/Enemies/Characters/Ranger.h"

void URangerAnimationInstance::InnerPlayMontage(EEnemyAnimState State)
{
	Super::InnerPlayMontage(State);
	if (State == EEnemyAnimState::GrenadeLaunch) UKismetSystemLibrary::K2_SetTimer(this, "LaunchGrenade", 1.1f, false);
}

void URangerAnimationInstance::OnMontageEnd(UAnimMontage* Montage, bool bIsInterrupted)
{
	Super::OnMontageEnd(Montage, bIsInterrupted);
	const TObjectPtr<UAnimMontage>* GrenadeLaunchMontage = Montages.Find(EEnemyAnimState::GrenadeLaunch);
	if (GrenadeLaunchMontage && Montage == GrenadeLaunchMontage->Get()) Proprietor->SetActionInMotion(false);
}

void URangerAnimationInstance::LaunchGrenade() const { Cast<ARanger>(Proprietor)->LaunchGrenade(); }
