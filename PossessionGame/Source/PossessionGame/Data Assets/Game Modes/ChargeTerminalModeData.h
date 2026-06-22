#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Data Assets/Base Data Assets/GameModeData.h"
#include "ChargeTerminalModeData.generated.h"

UCLASS()
class POSSESSIONGAME_API UChargeTerminalModeData : public UGameModeData
{
	GENERATED_BODY()
public:
	bool GetIsWallLasersExist() const { return WallLasersExist; }
	int32 GetChargeAmountRequired() const { return ChargeAmountRequired; }
	ECharacterType GetEnemyToSpawn() const { return EnemyToSpawn; }
private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	bool WallLasersExist = false;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	int32 ChargeAmountRequired = 2;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	ECharacterType EnemyToSpawn;
};
