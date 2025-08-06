#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TutorialManager.generated.h"

class AEnemy;

DECLARE_DELEGATE(FOnMinionDeathsCompleted);

UCLASS()
class POSSESSIONGAME_API UTutorialManager : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	FOnMinionDeathsCompleted OnMinionDeathsCompleted; 
	
	bool GetIsInitialized() const { return bIsInitialized; }
	
	void Init();
	void SetPlayerRef(const TObjectPtr<AEnemy>& InPlayer);
private:
	bool bIsInitialized = false;
	bool bPressKeyEnabled = false;
	int32 MinionDeathsRequired = 2;
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
	void MinionDeathOccured();
};
