#pragma once

#include "CoreMinimal.h"
#include "DelegateManager.generated.h"

enum class EUpgrade : uint8;

DECLARE_DELEGATE(FOnHandsReloadMontageEnd);
DECLARE_DELEGATE(FOnZoomToggle);
DECLARE_DELEGATE(FOnGraphicsSettingChange);
DECLARE_DELEGATE(FOnUpgradeSelected);
DECLARE_DELEGATE(FOnUpgradeSelectionFinished);
DECLARE_MULTICAST_DELEGATE(FOnEnemyDeath);
DECLARE_MULTICAST_DELEGATE(FOnPlayerDeath);
DECLARE_DELEGATE_OneParam(FOnPlayerDamageDealt, bool);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnEnemiesAmountDecrease, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FApplyUpgradeOnSelection, const EUpgrade);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnGlobalGraphicsSettingChange, uint8);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnConcealAbilityChange, TObjectPtr<AActor>);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnTargetChange, TObjectPtr<AActor>, bool);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnXRayThroughWallsChange, bool);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnWeaponAmmoChangeOneParam, int32);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnWeaponAmmoChangeTwoParams, int32, int32);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnWeaponAmmoChangeThreeParams, int32, int32, int32);

UCLASS()
class POSSESSIONGAME_API UDelegateManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	FOnHandsReloadMontageEnd OnHandsReloadMontageEnd;
	FOnZoomToggle OnZoomToggle;
	FOnGraphicsSettingChange OnGraphicsSettingChange;
	FOnUpgradeSelected OnUpgradeSelected;
	FOnUpgradeSelectionFinished OnUpgradeSelectionFinished;
	FOnEnemyDeath OnEnemyDeath;
	FOnPlayerDeath OnPlayerDeath;
	FOnPlayerDamageDealt OnPlayerDamageDealt;
	FOnEnemiesAmountDecrease OnEnemiesAmountDecrease;
	FApplyUpgradeOnSelection ApplyUpgradeOnSelection;
	FOnGlobalGraphicsSettingChange OnGlobalGraphicsSettingChange;
	FOnConcealAbilityChange OnConcealAbilityChange;
	FOnTargetChange OnTargetChange;
	FOnXRayThroughWallsChange OnXRayThroughWallsChange;
	FOnWeaponAmmoChangeOneParam OnWeaponAmmoChangeOneParam;
	FOnWeaponAmmoChangeTwoParams OnWeaponAmmoChangeTwoParams;
	FOnWeaponAmmoChangeThreeParams OnWeaponAmmoChangeThreeParams;
};
