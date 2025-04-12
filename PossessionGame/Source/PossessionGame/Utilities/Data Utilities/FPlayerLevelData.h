#pragma once

#include "FPlayerLevelData.generated.h"

USTRUCT()
struct FPlayerLevelData
{
	GENERATED_BODY()
	
	FPlayerLevelData();

	int32 GetDamageIncrease(const int32 InLevel);
private:
	UPROPERTY()
	TMap<int32, int32> DamageIncreases;
};
