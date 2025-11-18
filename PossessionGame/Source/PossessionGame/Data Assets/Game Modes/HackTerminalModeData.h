#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Data Assets/Base Data Assets/GameModeData.h"
#include "HackTerminalModeData.generated.h"

UCLASS()
class POSSESSIONGAME_API UHackTerminalModeData : public UGameModeData
{
	GENERATED_BODY()
public:
	int32 GetTerminalsAmountToHack() const { return TerminalsAmountToHack; }
	float GetEnemiesHpPool() const { return EnemiesHpPool; }
private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	int32 TerminalsAmountToHack = 3;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	float EnemiesHpPool = 3000.f;
};
