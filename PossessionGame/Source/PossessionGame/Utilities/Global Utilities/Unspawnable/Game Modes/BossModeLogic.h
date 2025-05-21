#pragma once

#include "CoreMinimal.h"
#include "WaveModeLogic.h"
#include "BossModeLogic.generated.h"

UCLASS()
class POSSESSIONGAME_API UBossModeLogic : public UGameModeBaseLogic
{
	GENERATED_BODY()

	bool bIsBossAlive = true;

	virtual void Init(const TObjectPtr<UWaveManager>& InWaveManager, const TObjectPtr<UGameModeData>& InGameModeData) override;
	virtual void Finalize() override;
	virtual void HandleSpawning() override;
	virtual void UpdateEnemiesAmount() override;
	virtual void SetupWaveManagerUI() override;
	virtual void FadeInObjectivePanel() const override;
	void BossDeath();
};
