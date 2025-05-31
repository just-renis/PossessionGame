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
	int32 UsbAmountProgress = 0;
	int32 UsbAmountRequired;
	UPROPERTY()
	TObjectPtr<UChargeTerminalModeData> Data;

	virtual void Init(const TObjectPtr<UWaveManager>& InWaveManager, const TObjectPtr<UGameModeData>& InGameModeData) override;
	virtual void Finalize() override;
	virtual void HandleSpawning() override;
	virtual void UpdateEnemiesAmount() override;
	virtual void UpdateTaskProgress() override;
	virtual void FadeInObjectivePanel() const override;
	void SetData();
};
