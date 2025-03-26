#pragma once

#include "CoreMinimal.h"
#include "DelegateManager.generated.h"

enum class EUpgrade : uint8;
enum class ECharacterType : uint8;

DECLARE_DELEGATE(FOnHandsReloadMontageEnd);
DECLARE_DELEGATE(FOnZoomToggle);
DECLARE_DELEGATE(FOnGraphicsSettingChange);
DECLARE_DELEGATE(FOnUpgradeSelectionOpened);
DECLARE_DELEGATE(FOnUpgradeSelected);
DECLARE_DELEGATE(FOnUpgradeSelectionFinished);
DECLARE_DELEGATE(FDestroyTutorialForceField);
DECLARE_DELEGATE(FOpenTutorialGate);
DECLARE_DELEGATE(FStopShowingHint);
DECLARE_DELEGATE_OneParam(FOnShowPressAnyKey, const bool);
DECLARE_DELEGATE_OneParam(FOnPlayerDamageDealt, bool);
DECLARE_DELEGATE_OneParam(FSpawnTutorialEnemy, const ECharacterType);
DECLARE_DELEGATE_OneParam(FStartTutorialDroneMoveToLocation, const int32);
DECLARE_DELEGATE_OneParam(FSendTargetToTutorialDrone, const TObjectPtr<AActor>&);
DECLARE_DELEGATE_OneParam(FOnCheckDistanceToTutorialDrone, const int32);
DECLARE_DELEGATE_OneParam(FOnShowHint, const int32);
DECLARE_DELEGATE_OneParam(FOnShowDialogue, const int32);
DECLARE_DELEGATE_OneParam(FOnTutorialStepChange, const int32);
DECLARE_MULTICAST_DELEGATE(FOnEnemyDeath);
DECLARE_MULTICAST_DELEGATE(FOnPlayerDeath);
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
	FOnUpgradeSelectionOpened OnUpgradeSelectionOpened;
	FOnUpgradeSelected OnUpgradeSelected;
	FOnUpgradeSelectionFinished OnUpgradeSelectionFinished;
	FDestroyTutorialForceField DestroyTutorialForceField;
	FOpenTutorialGate OpenTutorialGate;
	FStopShowingHint StopShowingHint;
	FOnShowPressAnyKey OnShowPressAnyKey;
	FOnPlayerDamageDealt OnPlayerDamageDealt;
	FSpawnTutorialEnemy SpawnTutorialEnemy;
	FStartTutorialDroneMoveToLocation StartTutorialDroneMoveToLocation;
	FSendTargetToTutorialDrone SendTargetToTutorialDrone;
	FOnCheckDistanceToTutorialDrone OnCheckDistanceToTutorialDrone;
	FOnShowHint OnShowHint;
	FOnShowDialogue OnShowDialogue;
	FOnTutorialStepChange OnTutorialStepChange;
	FOnEnemyDeath OnEnemyDeath;
	FOnPlayerDeath OnPlayerDeath;
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
