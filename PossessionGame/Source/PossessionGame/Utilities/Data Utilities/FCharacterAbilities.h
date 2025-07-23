#pragma once

#include "FCharacterAbilities.generated.h"

USTRUCT()
struct FCharacterAbilities
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<int32> Abilities;
};
