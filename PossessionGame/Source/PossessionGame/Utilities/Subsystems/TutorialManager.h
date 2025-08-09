#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Enums/EGameMode.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "Subsystems/WorldSubsystem.h"
#include "TutorialManager.generated.h"

class AEnemy;

DECLARE_DELEGATE(FStartTutorialBotAnimation);
DECLARE_DELEGATE(FEnableMeleeChecking);
DECLARE_DELEGATE(FOnShowHint);
DECLARE_DELEGATE(FOnStopShowingHint);
DECLARE_DELEGATE(FOnFpsExplanationTriggered);
DECLARE_DELEGATE_TwoParams(FSpawnTutorialEnemy, const ECharacterType, const EGameMode);
DECLARE_MULTICAST_DELEGATE(FOnAlertStart);
DECLARE_MULTICAST_DELEGATE(FOnMinionDeathCompleted);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnShowPressKeyToContinue, const bool);

UCLASS()
class POSSESSIONGAME_API UTutorialManager : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	FStartTutorialBotAnimation StartTutorialBotAnimation;
	FEnableMeleeChecking EnableMeleeChecking;
	FOnShowHint OnShowHint;
	FOnStopShowingHint OnStopShowingHint;
	FOnFpsExplanationTriggered OnFpsExplanationTriggered;
	FSpawnTutorialEnemy SpawnTutorialEnemy;
	FOnAlertStart OnAlertStart;
	FOnMinionDeathCompleted OnMinionDeathCompleted;
	FOnShowPressKeyToContinue OnShowPressKeyToContinue;
	
	bool GetIsInitialized() const { return bIsInitialized; }
	
	void Init();
	void SetPlayerRef(const TObjectPtr<AEnemy>& InPlayer);
private:
	bool bIsInitialized = false;
	bool bPressKeyEnabled = false;
	FVector LastPlayerLocation;
	UPROPERTY()
	TObjectPtr<AEnemy> Player;
	UPROPERTY()
	FTimerHandle TutorialHandle;
	UPROPERTY()
	FTimerHandle MoveTutorialDisappearHandle;

	void SetPressKeyState(const bool bState) { bPressKeyEnabled = bState; }
	void CheckHijacked();
	void CheckMovement();
	void StopShowingHint() const;
	void PressKeyToContinue();
	void MinionDeathOccured() const;
};
