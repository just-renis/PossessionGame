#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TutorialManager.generated.h"

class AEnemy;

UCLASS()
class POSSESSIONGAME_API UTutorialManager : public UWorldSubsystem
{
	GENERATED_BODY()
public:
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

	void CheckHijacked();
	void CheckMovement();
	void StopShowingHint() const;
	void PressKeyToContinue();
	void SetPressKeyState(const bool bState) { bPressKeyEnabled = bState; }
};
