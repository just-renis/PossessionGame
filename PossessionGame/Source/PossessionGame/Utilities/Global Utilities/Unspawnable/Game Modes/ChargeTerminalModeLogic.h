#pragma once

#include "CoreMinimal.h"
#include "GameModeBaseLogic.h"
#include "ChargeTerminalModeLogic.generated.h"

class UChargeTerminalModeData;

UCLASS()
class POSSESSIONGAME_API UChargeTerminalModeLogic : public UGameModeBaseLogic
{
	GENERATED_BODY()

	bool bIsTerminalCharged = false;
	float TerminalHackProgress = 0.f;
	float TerminalHackRequired;
	UPROPERTY()
	TObjectPtr<UChargeTerminalModeData> Data;

	virtual void Init(const TObjectPtr<UWaveManager>& InWaveManager, const TObjectPtr<UGameModeData>& InGameModeData) override;
	virtual void Finalize() override;
	virtual void HandleSpawning() override;
	virtual void UpdateEnemiesAmount() override;
	virtual void UpdateTaskProgress() override;
	void SetData();
};
