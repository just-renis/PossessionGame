#pragma once

#include "CoreMinimal.h"
#include "GameModeBaseLogic.h"
#include "HackTerminalModeLogic.generated.h"

UCLASS()
class POSSESSIONGAME_API UHackTerminalModeLogic : public UGameModeBaseLogic
{
	GENERATED_BODY()

	bool bIsHackFinished = false;
	int32 TerminalsLeftToHack = 3;
	float TerminalHackProgress = 0.f;
	float TerminalHackRequired = 5.f;

	virtual void Init(const TObjectPtr<UWaveManager> InWaveManager) override;
	virtual void Finalize() override;
	virtual void HandleSpawning() override;
	virtual void UpdateTaskProgress() override;
	virtual void UpdateEnemiesAmount(const int32 InXp) override;
	void StartTerminalHack();
	void FinishTerminalHack();
};
