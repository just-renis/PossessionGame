#pragma once

#include "CoreMinimal.h"
#include "DelegateManager.generated.h"

enum class EGameMode : uint8;
enum class EUpgrade : uint8;
enum class ECharacterType : uint8;
enum class EVariation : uint8;

DECLARE_DELEGATE(FOnHandsReloadMontageEnd);
DECLARE_DELEGATE(FOnGraphicsSettingChange);
DECLARE_DELEGATE(FOnUpgradeSelectionOpened);
DECLARE_DELEGATE(FOnUpgradeSelected);
DECLARE_DELEGATE(FOnUpgradeSelectionFinished);
DECLARE_DELEGATE(FDestroyTutorialForceField);
DECLARE_DELEGATE(FStopShowingHint);
DECLARE_DELEGATE(FOnChargeTerminalModeCompletion);
DECLARE_DELEGATE(FOnUsbCollected);
DECLARE_DELEGATE(FOnUsbInserted);
DECLARE_DELEGATE(FOnArtifactCollected);
DECLARE_DELEGATE(FOnSecretDoorOpened);
DECLARE_DELEGATE_OneParam(FOnPlayerDamageDealt, bool);
DECLARE_DELEGATE_OneParam(FSpawnTutorialEnemy, const ECharacterType);
DECLARE_DELEGATE_OneParam(FSendTargetToTutorialDrone, const TObjectPtr<AActor>&);
DECLARE_DELEGATE_OneParam(FOnCheckDistanceToTutorialDrone, const int32);
DECLARE_DELEGATE_OneParam(FOnShowHint, const int32);
DECLARE_DELEGATE_OneParam(FOnShowDialogue, const int32);
DECLARE_DELEGATE_OneParam(FOnMinionBossStomp, const TSubclassOf<UCameraShakeBase>&);
DECLARE_DELEGATE_OneParam(FOnSecretRewardCollected, const int32);
DECLARE_DELEGATE_OneParam(FOnVariationSelectionEnd, const ECharacterType);
DECLARE_DELEGATE_TwoParams(FStartTutorialDroneMoveToLocation, const int32, const bool);
DECLARE_DELEGATE_TwoParams(FOnAbilityEnd, const int32, const bool);
DECLARE_DELEGATE_ThreeParams(FOnAbilityUse, const int32, const bool, const bool);
DECLARE_MULTICAST_DELEGATE(FOnTerminalInteraction);
DECLARE_MULTICAST_DELEGATE(FOnPlayerDeath);
DECLARE_MULTICAST_DELEGATE(FOnTutorialEnd);
DECLARE_MULTICAST_DELEGATE(FOnHackModeStart);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnEnemiesAmountDecrease, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FApplyUpgradeOnSelection, const EUpgrade);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnGlobalGraphicsSettingChange, uint8);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnConcealAbilityChange, TObjectPtr<AActor>);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnEnemyDeath, const int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLevelUp, const int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnXRayThroughWallsChange, bool);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnWeaponAmmoChangeOneParam, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnGameModeInitiator, const EGameMode);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnTerminalHackFinish, const EGameMode);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnGameModeFinished, const EGameMode);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnShowPressKeyToContinue, const bool);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnWeaponAmmoChangeTwoParams, int32, int32);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnVariationSelection, const ECharacterType, const EVariation);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnTargetChange, TObjectPtr<AActor>, bool);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnWeaponAmmoChangeThreeParams, int32, int32, int32);

UCLASS()
class POSSESSIONGAME_API UDelegateManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	FOnHandsReloadMontageEnd OnHandsReloadMontageEnd;
	FOnGraphicsSettingChange OnGraphicsSettingChange;
	FOnUpgradeSelectionOpened OnUpgradeSelectionOpened;
	FOnUpgradeSelected OnUpgradeSelected;
	FOnUpgradeSelectionFinished OnUpgradeSelectionFinished;
	FDestroyTutorialForceField DestroyTutorialForceField;
	FStopShowingHint StopShowingHint;
	FOnChargeTerminalModeCompletion OnChargeTerminalModeCompletion;
	FOnUsbCollected OnUsbCollected;
	FOnUsbInserted OnUsbInserted;
	FOnArtifactCollected OnArtifactCollected;
	FOnSecretDoorOpened OnSecretDoorOpened;
	FOnPlayerDamageDealt OnPlayerDamageDealt;
	FSpawnTutorialEnemy SpawnTutorialEnemy;
	FSendTargetToTutorialDrone SendTargetToTutorialDrone;
	FOnCheckDistanceToTutorialDrone OnCheckDistanceToTutorialDrone;
	FOnShowHint OnShowHint;
	FOnShowDialogue OnShowDialogue;
	FOnMinionBossStomp OnMinionBossStomp;
	FOnSecretRewardCollected OnSecretRewardCollected;
	FOnVariationSelectionEnd OnVariationSelectionEnd;
	FStartTutorialDroneMoveToLocation StartTutorialDroneMoveToLocation;
	FOnAbilityEnd OnAbilityEnd;
	FOnAbilityUse OnAbilityUse;
	FOnTerminalInteraction OnTerminalInteraction;
	FOnPlayerDeath OnPlayerDeath;
	FOnTutorialEnd OnTutorialEnd;
	FOnHackModeStart OnHackModeStart;
	FOnEnemiesAmountDecrease OnEnemiesAmountDecrease;
	FApplyUpgradeOnSelection ApplyUpgradeOnSelection;
	FOnGlobalGraphicsSettingChange OnGlobalGraphicsSettingChange;
	FOnConcealAbilityChange OnConcealAbilityChange;
	FOnEnemyDeath OnEnemyDeath;
	FOnLevelUp OnLevelUp;
	FOnXRayThroughWallsChange OnXRayThroughWallsChange;
	FOnWeaponAmmoChangeOneParam OnWeaponAmmoChangeOneParam;
	FOnGameModeInitiator OnGameModeInitiator;
	FOnTerminalHackFinish OnTerminalHackFinish;
	FOnGameModeFinished OnGameModeFinished;
	FOnShowPressKeyToContinue OnShowPressKeyToContinue;
	FOnWeaponAmmoChangeTwoParams OnWeaponAmmoChangeTwoParams;
	FOnVariationSelection OnVariationSelection;
	FOnTargetChange OnTargetChange;
	FOnWeaponAmmoChangeThreeParams OnWeaponAmmoChangeThreeParams;

	bool GetIsUsbDroppable() const { return bIsUsbDroppable; }

	void SetIsUsbDroppable(const bool bState) { bIsUsbDroppable = bState; }
private:
	bool bIsUsbDroppable = false;
};
