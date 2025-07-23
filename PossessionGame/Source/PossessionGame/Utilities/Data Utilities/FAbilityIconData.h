#pragma once

#include "FAbilityIconData.generated.h"

USTRUCT()
struct FAbilityIconData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	int32 AbilityIndex = -1;
	UPROPERTY(EditAnywhere)
	FText Details;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> AbilityIcon;
};
