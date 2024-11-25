#include "RangerAnimationInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/AI/Enemies/Characters/Ranger.h"

void URangerAnimationInstance::InnerPlayMontage(EEnemyAnimState State)
{
	Super::InnerPlayMontage(State);
	if (State == EEnemyAnimState::GrenadeLaunch) UKismetSystemLibrary::K2_SetTimer(this, "LaunchGrenade", 0.55f, false);
}

//void URangerAnimationInstance::SetProprietor() { Proprietor = Cast<ARanger>(GetOwningActor()); }

void URangerAnimationInstance::LaunchGrenade() { Cast<ARanger>(Proprietor)->LaunchGrenade(); }
