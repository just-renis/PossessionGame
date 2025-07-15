#include "PoisonArea.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EVariation.h"
#include "PossessionGame/Utilities/Subsystems/PlayerDataManager.h"
#include "PossessionGame/Utilities/Subsystems/VariationManager.h"

void APoisonArea::DealOvertimeDamage()
{
	if (Targets.IsEmpty()) return;
	if (EVariation::TricksterVariationPurple == GetWorld()->GetSubsystem<UVariationManager>()->GetCurrentVariation(ECharacterType::Trickster)) GetGameInstance()->GetSubsystem<UPlayerDataManager>()->AddPoisonAreaUptime(DamageTick);
	for (int32 i = 0; i < Targets.Num(); i++) if (Targets[i]) Targets[i]->ReceiveDamage(Damage, DamageReason);
}
