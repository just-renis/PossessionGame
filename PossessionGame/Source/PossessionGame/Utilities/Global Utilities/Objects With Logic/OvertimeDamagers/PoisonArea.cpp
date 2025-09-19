#include "PoisonArea.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Enums/EUpgradeAttribute.h"
#include "PossessionGame/Enums/EVariation.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PlayerDataManager.h"
#include "PossessionGame/Utilities/Subsystems/VariationManager.h"

void APoisonArea::Setup(const bool bInIsPlayerObject, const float InDamage, const float InDamageTick, const float InLifeSpan, const float InRadiusSize, const EDamageReason InDamageReason)
{
	bIsActionSpeedReduceUpgradeTaken = GetGameInstance()->GetSubsystem<UDelegateManager>()->RequestSpecialUpgradeAvailability.Execute(EUpgrade::TricksterAbility1, EUpgradeAttribute::Special1);
	Super::Setup(bInIsPlayerObject, InDamage, InDamageTick, InLifeSpan, InRadiusSize, InDamageReason);
}

void APoisonArea::DealOvertimeDamage()
{
	if (Targets.IsEmpty()) return;
	if (EVariation::TricksterVariationPurple == GetWorld()->GetSubsystem<UVariationManager>()->GetCurrentVariation(ECharacterType::Trickster)) GetGameInstance()->GetSubsystem<UPlayerDataManager>()->AddPoisonAreaUptime(DamageTick);
	for (int32 i = 0; i < Targets.Num(); i++)
	{
		if (!Targets[i]) continue;
		Targets[i]->ReceiveDamage(Damage, DamageReason);
	}
}

void APoisonArea::AddTarget(const TObjectPtr<AEnemy>& InTarget)
{
	Super::AddTarget(InTarget);
	if (bIsActionSpeedReduceUpgradeTaken) InTarget->ChangeOverlappingPoisonAreasInState(true);
}

void APoisonArea::RemoveTarget(const TObjectPtr<AEnemy>& InTarget)
{
	Super::RemoveTarget(InTarget);
	if (bIsActionSpeedReduceUpgradeTaken) InTarget->ChangeOverlappingPoisonAreasInState(false);
}
