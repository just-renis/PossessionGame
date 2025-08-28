#pragma once

#include "CoreMinimal.h"
#include "GameModeBaseLogic.h"
#include "HackTerminalModeLogic.generated.h"

enum class EGameMode : uint8;
class UHackTerminalModeData;

UCLASS()
class POSSESSIONGAME_API UHackTerminalModeLogic : public UGameModeBaseLogic
{
	GENERATED_BODY()

	bool bIsHackFinished = false;
	int32 TerminalsHackedAmount = 0;
	float TerminalHackProgress = 0.f;
	float TerminalHackRequired;
	EGameMode CurrentTerminalRoomMode;
	UPROPERTY()
	TObjectPtr<UHackTerminalModeData> Data;
	UPROPERTY()
	FTimerHandle OpenUpgradeSelectionHandle;
	UPROPERTY()
	FTimerHandle TaskProgressHandle;

	void SetTerminalRoomMode(const EGameMode InTerminalRoomMode) { CurrentTerminalRoomMode = InTerminalRoomMode; }
	virtual void Init(const TObjectPtr<UWaveManager>& InWaveManager, const TObjectPtr<UGameModeData>& InGameModeData) override;
	virtual void Finalize() override;
	virtual void HandleSpawning() override;
	virtual void UpdateTaskProgress() override;
	virtual void UpdateEnemiesAmount() override;
	virtual void FadeInObjectivePanel() const override;
	void StartTerminalHack();
	void FinishTerminalHack();
	void SetData();
};
