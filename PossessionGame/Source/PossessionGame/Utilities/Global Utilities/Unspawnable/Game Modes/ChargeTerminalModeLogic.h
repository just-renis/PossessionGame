#pragma once

#include "CoreMinimal.h"
#include "GameModeBaseLogic.h"
#include "ChargeTerminalModeLogic.generated.h"

UCLASS()
class POSSESSIONGAME_API UChargeTerminalModeLogic : public UGameModeBaseLogic
{
	GENERATED_BODY()

	bool bIsTerminalCharged = false;
	float TerminalHackProgress = 0.f;
	float TerminalHackRequired = 5.f;

	virtual void Init(const TObjectPtr<UWaveManager> InWaveManager) override;
	virtual void Finalize() override;
	virtual void HandleSpawning() override;
	virtual void UpdateEnemiesAmount(const int32 InXp) override;
	virtual void UpdateTaskProgress() override;
};
