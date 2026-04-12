#pragma once

#include "CoreMinimal.h"
#include "GameModeBaseLogic.h"
#include "MultipleWardenModeLogic.generated.h"

enum class EGameMode : uint8;
class UMultipleWardenModeData;

UCLASS()
class POSSESSIONGAME_API UMultipleWardenModeLogic : public UGameModeBaseLogic
{
	GENERATED_BODY()
	
	bool bIsTerminalsCharged = false;
	bool bIsUsbRequirementFilled = false;
	int32 ChargeAmount = 0;
	UPROPERTY()
	TObjectPtr<UMultipleWardenModeData> Data;

	virtual void Init(const TObjectPtr<UWaveManager>& InWaveManager, const TObjectPtr<UGameModeData>& InGameModeData) override;
	virtual void Finalize() override;
	virtual void HandleSpawning() override;
	virtual void UpdateEnemiesAmount() override;
	virtual void UpdateTaskProgress() override;
	virtual void FadeInObjectivePanel() const override;
	void UsbRequirementFilled();
	void ChargesCleared();
	void InitiateNextCharge();
	void SpawnMiniBosses();
	void SpawnMiniBoss(const EGameMode InGameModeSpawn);
};
