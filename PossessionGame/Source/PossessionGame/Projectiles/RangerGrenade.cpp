#include "RangerGrenade.h"
#include "Components/AudioComponent.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/Enums/EUpgrade.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PlayerDataManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"

ARangerGrenade::ARangerGrenade()
{
	Sounds.Add(ESound::RangerGrenadeExplosion, CreateDefaultSubobject<UAudioComponent>(TEXT("grenadeExplosionSound")));
	Sounds[ESound::RangerGrenadeExplosion]->SetAutoActivate(false);
	Sounds[ESound::RangerGrenadeExplosion]->SetupAttachment(Mesh);
}

float ARangerGrenade::AI_UpgradeDamageAppliance()
{
	if (!GetGameInstance()->GetSubsystem<UDelegateManager>()->RequestSpecialUpgradeAvailability.Execute(EUpgrade::RangerAbility1, EUpgradeAttribute::Special1)) return 0.f;
	const int32 AbilitiesUpgradesLevel = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetRunSaveSlot()->GetCurrentLevelSumOfCharacterAbilities(ProjectileData.GetProprietor()->GetCharacterName());
	return DefaultDamage * AbilitiesUpgradesLevel * 0.1f;
}

float ARangerGrenade::P_UpgradeDamageAppliance()
{
	return DefaultDamage * GetGameInstance()->GetSubsystem<UPlayerDataManager>()->GetRangerGrenadeSpecialBoost();
}

void ARangerGrenade::PlayImpactSound() { PlaySound(ESound::RangerGrenadeExplosion); }
