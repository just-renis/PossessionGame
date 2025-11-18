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

	int32 TerminalsHackedAmount = 0;
	float UnreservedEnemiesHpPool;
	float ReservedEnemiesHpPool;
	EGameMode CurrentTerminalRoomMode;
	FTimerHandle OpenUpgradeSelectionHandle;
	UPROPERTY()
	TObjectPtr<UHackTerminalModeData> Data;

	void SetTerminalRoomMode(const EGameMode InTerminalRoomMode) { CurrentTerminalRoomMode = InTerminalRoomMode; }
	virtual void Init(const TObjectPtr<UWaveManager>& InWaveManager, const TObjectPtr<UGameModeData>& InGameModeData) override;
	virtual void Finalize() override;
	virtual void HandleSpawning() override;
	virtual void UpdateEnemiesAmount() override;
	virtual void FadeInObjectivePanel() const override;
	void StartTerminalHack();
	void FinishTerminalHack();
	void StopSpawning();
	void UpdateHpPoolBar(const float InHp);
};
