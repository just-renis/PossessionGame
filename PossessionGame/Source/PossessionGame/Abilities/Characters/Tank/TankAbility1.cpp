#include "TankAbility1.h"
#include "PossessionGame/AI/Enemies/Characters/Tank.h"
#include "PossessionGame/Enums/EAchievement.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/PoolObject.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PoolingManager.h"
#include "PossessionGame/Weapons/Management/Weapon.h"

void UTankAbility1::AI_Execute()
{
	Proprietor->AI_SpawnShield();
	Super::AI_Execute();
}

void UTankAbility1::P_Execute()
{
	//(void)DelegateManager->OnAchievementCompleted.ExecuteIfBound(EAchievement::FIRST_ACHIEVEMENT);
	if (const TObjectPtr<APoolObject> ShieldProjector = GetWorld()->GetSubsystem<UPoolingManager>()->SpawnObjectFromPool(EPoolObject::TankShieldProjector, Proprietor->GetActorLocation())) ShieldProjector->Init();
	Super::P_Execute();
}

bool UTankAbility1::IsProprietorPossessed() { return Proprietor->GetIsHijacked(); }

void UTankAbility1::SetProprietor(const TObjectPtr<AEnemy>& InAbilityProprietor) { Proprietor = Cast<ATank>(InAbilityProprietor); }
