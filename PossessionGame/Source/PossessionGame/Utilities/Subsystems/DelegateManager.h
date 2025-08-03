#pragma once

#include "CoreMinimal.h"
#include "DelegateManager.generated.h"

enum class EGameMode : uint8;
enum class EUpgrade : uint8;
enum class ECharacterType : uint8;
enum class EVariation : uint8;
enum class EWidget : uint8;
class UUpgradeData;

DECLARE_DELEGATE(FOnHandsReloadMontageEnd);
DECLARE_DELEGATE(FOnGraphicsSettingChange);
DECLARE_DELEGATE(FOnUpgradeSelectionOpened);
DECLARE_DELEGATE(FOnUpgradeSelectionFinished);
DECLARE_DELEGATE(FOnChargeTerminalModeCompletion);
DECLARE_DELEGATE(FOnUsbCollected);
DECLARE_DELEGATE(FOnUsbInserted);
DECLARE_DELEGATE(FOnArtifactCollected);
DECLARE_DELEGATE(FOnSecretDoorOpened);
DECLARE_DELEGATE(FUpdateGameModeEnemiesAmount);
DECLARE_DELEGATE(FOnBossSpawn);
DECLARE_DELEGATE(FOnOutOfAmmo);
DECLARE_DELEGATE(FOnGameLaunchFinished);
DECLARE_DELEGATE(FOnMusicSoundtrackChangeRequired);
DECLARE_DELEGATE(FMeleeAttackOccured);
DECLARE_DELEGATE_OneParam(FOnPlayerDamageDealt, bool);
DECLARE_DELEGATE_OneParam(FOnCheckDistanceToTutorialDrone, const int32);
DECLARE_DELEGATE_OneParam(FOnMinionBossStomp, const TSubclassOf<UCameraShakeBase>&);
DECLARE_DELEGATE_OneParam(FOnUpgradeCardSelected, const TObjectPtr<UUpgradeData>&);
DECLARE_DELEGATE_OneParam(FApplyUpgradeOnSelection, const EUpgrade);
DECLARE_DELEGATE_OneParam(FRequirePlayerAnnouncementInfo, const FText&);
DECLARE_DELEGATE_OneParam(FChangeMusicPlayerState, const bool);
DECLARE_DELEGATE_OneParam(FOnCoinCollected, const int32);
DECLARE_DELEGATE_OneParam(FOnXpReceived, const int32);
DECLARE_DELEGATE_OneParam(FApplySecretBoostHealth, const int32);
DECLARE_DELEGATE_OneParam(FOnOpenUnlockAbilityPopup, const int32);
DECLARE_DELEGATE_TwoParams(FStartTutorialDroneMoveToLocation, const int32, const bool);
DECLARE_DELEGATE_TwoParams(FOnAbilityEnd, const int32, const bool);
DECLARE_DELEGATE_TwoParams(FOnEnemyDeath, const int32, const float);
DECLARE_DELEGATE_TwoParams(FOnBossDamaged, const float, const float);
DECLARE_DELEGATE_TwoParams(FOnVariationSelectionEnd, const ECharacterType, const EWidget);
DECLARE_DELEGATE_ThreeParams(FOnAbilityUse, const int32, const bool, const bool);
DECLARE_MULTICAST_DELEGATE(FOnTerminalInteraction);
DECLARE_MULTICAST_DELEGATE(FOnPlayerDeath);
DECLARE_MULTICAST_DELEGATE(FOnHackModeStart);
DECLARE_MULTICAST_DELEGATE(FOnBossDeath);
DECLARE_MULTICAST_DELEGATE(FOnMainTutorialEnd);
DECLARE_MULTICAST_DELEGATE(FOnInteractionTry);
DECLARE_MULTICAST_DELEGATE(FOnSecretBoostSelected);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnGlobalGraphicsSettingChange, uint8);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnConcealAbilityChange, TObjectPtr<AActor>);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLevelUp, const int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnXRayThroughWallsChange, bool);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnWeaponAmmoChangeOneParam, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnGameModeInitiator, const EGameMode);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnTerminalHackFinish, const EGameMode);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnGameModeFinished, const EGameMode);
DECLARE_MULTICAST_DELEGATE_OneParam(FDestroyTutorialForceField, const int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnVariationUnlocked, const EVariation);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnWeaponAmmoChangeTwoParams, int32, int32);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnVariationSelection, const ECharacterType, const EVariation);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnTargetChange, const TObjectPtr<AActor>&, bool);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnKeyBindChange, const FName&, const FText&);

//----------------------------------------------------TUTORIAL DELEGATES----------------------------------------------------
DECLARE_DELEGATE(FStopShowingHint);
DECLARE_DELEGATE(FOnShowHint);
DECLARE_DELEGATE(FStartTutorialBotAnimation);
DECLARE_DELEGATE(FEnableMeleeChecking);
DECLARE_DELEGATE_TwoParams(FSpawnTutorialEnemy, const ECharacterType, const EGameMode);
DECLARE_MULTICAST_DELEGATE(FOnAlertStart);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnShowPressKeyToContinue, const bool);

UCLASS()
class POSSESSIONGAME_API UDelegateManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	FOnHandsReloadMontageEnd OnHandsReloadMontageEnd;
	FOnGraphicsSettingChange OnGraphicsSettingChange;
	FOnUpgradeSelectionOpened OnUpgradeSelectionOpened;
	FOnUpgradeSelectionFinished OnUpgradeSelectionFinished;
	FOnChargeTerminalModeCompletion OnChargeTerminalModeCompletion;
	FOnUsbCollected OnUsbCollected;
	FOnUsbInserted OnUsbInserted;
	FOnArtifactCollected OnArtifactCollected;
	FOnSecretDoorOpened OnSecretDoorOpened;
	FUpdateGameModeEnemiesAmount UpdateGameModeEnemiesAmount;
	FOnBossSpawn OnBossSpawn;
	FOnOutOfAmmo OnOutOfAmmo;
	FOnGameLaunchFinished OnGameLaunchFinished;
	FOnMusicSoundtrackChangeRequired OnMusicSoundtrackChangeRequired;
	FMeleeAttackOccured MeleeAttackOccured;
	FOnPlayerDamageDealt OnPlayerDamageDealt;
	FOnEnemyDeath OnEnemyDeath;
	FOnBossDamaged OnBossDamaged;
	FOnVariationSelectionEnd OnVariationSelectionEnd;
	FOnCheckDistanceToTutorialDrone OnCheckDistanceToTutorialDrone;
	FOnMinionBossStomp OnMinionBossStomp;
	FOnUpgradeCardSelected OnUpgradeCardSelected;
	FApplyUpgradeOnSelection ApplyUpgradeOnSelection;
	FRequirePlayerAnnouncementInfo RequirePlayerAnnouncementInfo;
	FChangeMusicPlayerState ChangeMusicPlayerState;
	FOnCoinCollected OnCoinCollected;
	FOnXpReceived OnXpReceived;
	FApplySecretBoostHealth ApplySecretBoostHealth;
	FOnOpenUnlockAbilityPopup OnOpenUnlockAbilityPopup;
	FStartTutorialDroneMoveToLocation StartTutorialDroneMoveToLocation;
	FOnAbilityEnd OnAbilityEnd;
	FOnAbilityUse OnAbilityUse;
	FOnTerminalInteraction OnTerminalInteraction;
	FOnPlayerDeath OnPlayerDeath;
	FOnHackModeStart OnHackModeStart;
	FOnBossDeath OnBossDeath;
	FOnMainTutorialEnd OnMainTutorialEnd;
	FOnInteractionTry OnInteractionTry;
	FOnSecretBoostSelected OnSecretBoostSelected;
	FOnGlobalGraphicsSettingChange OnGlobalGraphicsSettingChange;
	FOnConcealAbilityChange OnConcealAbilityChange;
	FOnLevelUp OnLevelUp;
	FOnXRayThroughWallsChange OnXRayThroughWallsChange;
	FOnWeaponAmmoChangeOneParam OnWeaponAmmoChangeOneParam;
	FOnGameModeInitiator OnGameModeInitiator;
	FOnTerminalHackFinish OnTerminalHackFinish;
	FOnGameModeFinished OnGameModeFinished;
	FDestroyTutorialForceField DestroyTutorialForceField;
	FOnVariationUnlocked OnVariationUnlocked;
	FOnWeaponAmmoChangeTwoParams OnWeaponAmmoChangeTwoParams;
	FOnVariationSelection OnVariationSelection;
	FOnTargetChange OnTargetChange;
	FOnKeyBindChange OnKeyBindChange;

	//----------------------------------------------------TUTORIAL DELEGATES----------------------------------------------------
	FStopShowingHint StopShowingHint;
	FOnShowHint OnShowHint;
	FStartTutorialBotAnimation StartTutorialBotAnimation;
	FEnableMeleeChecking EnableMeleeChecking;
	FSpawnTutorialEnemy SpawnTutorialEnemy;
	FOnAlertStart OnAlertStart;
	FOnShowPressKeyToContinue OnShowPressKeyToContinue;
};
