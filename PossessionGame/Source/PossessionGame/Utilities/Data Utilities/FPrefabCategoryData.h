#pragma once

#include "FPrefabCategoryData.generated.h"

class APrefab;

USTRUCT()
struct POSSESSIONGAME_API FPrefabCategoryData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<APrefab>> PrefabClasses;
};
