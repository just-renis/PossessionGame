#pragma once

#include "CoreMinimal.h"
#include "GameModeBaseLogic.h"
#include "HackTerminalModeLogic.generated.h"

class UHackTerminalModeData;

UCLASS()
class POSSESSIONGAME_API UHackTerminalModeLogic : public UGameModeBaseLogic
{
	GENERATED_BODY()

	bool bIsHackFinished = false;
	int32 TerminalsLeftToHack;
	float TerminalHackProgress = 0.f;
	float TerminalHackRequired ;
	UPROPERTY()
	TObjectPtr<UHackTerminalModeData> Data;

	virtual void Init(const TObjectPtr<UWaveManager>& InWaveManager, const TObjectPtr<UGameModeData>& InGameModeData) override;
	virtual void Finalize() override;
	virtual void HandleSpawning() override;
	virtual void UpdateTaskProgress() override;
	virtual void UpdateEnemiesAmount() override;
	void StartTerminalHack();
	void FinishTerminalHack();
	void SetData();
};
