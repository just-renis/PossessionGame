#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PlayerLevelData.generated.h"

UCLASS()
class POSSESSIONGAME_API UPlayerLevelData : public UDataAsset
{
	GENERATED_BODY()
public:
	float GetXpNeeded(const int32 InLevel);
private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TMap<int32, float> LevelXpNeeded;
};
