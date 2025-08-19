#pragma once

#include "FUpgradeAttributesData.generated.h"

enum class EUpgradeAttribute : uint8;

USTRUCT()
struct FUpgradeAttributesData
{
	GENERATED_BODY()
	
	UPROPERTY()
	TMap<EUpgradeAttribute, float> Attributes;
};
