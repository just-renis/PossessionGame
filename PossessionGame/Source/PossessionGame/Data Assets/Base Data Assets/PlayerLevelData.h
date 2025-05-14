#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PlayerLevelData.generated.h"

UCLASS()
class POSSESSIONGAME_API UPlayerLevelData : public UDataAsset
{
	GENERATED_BODY()
public:
	int32 GetDamageIncrease(const int32 InLevel);
	int32 GetHpIncrease(const int32 InLevel);
private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TMap<int32, int32> DamageIncreases;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TMap<int32, int32> HpIncreases;
};
