#include "SoldierBoss.h"
#include "Components/WidgetComponent.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EEnemyHpBar.h"
#include "PossessionGame/Enums/EWeapon.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/WeaponManager.h"

ASoldierBoss::ASoldierBoss()
{
	if (AI_HpBarComponent)
	{
		AI_HpBarComponent->DestroyComponent();
		AI_HpBarComponent = nullptr;
	}
}

void ASoldierBoss::AI_SetupWeapon()
{
	Weapon = GetWorld()->GetGameInstance()->GetSubsystem<UWeaponManager>()->GetWeapon(GetWorld(), EWeapon::SoldierBossWeapon, GetMesh());
	Super::AI_SetupWeapon();
}

void ASoldierBoss::BeginPlay()
{
	CharacterName = ECharacterType::SoldierBoss;
	bIsBoss = true;
	Super::BeginPlay();
	(void)DelegateManager->OnGlobalHpBarSpawn.ExecuteIfBound(EEnemyHpBar::Boss);
}
