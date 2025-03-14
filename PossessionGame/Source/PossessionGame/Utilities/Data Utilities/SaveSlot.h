#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveSlot.generated.h"

enum class EUpgrade : uint8;

UCLASS()
class POSSESSIONGAME_API USaveSlot : public USaveGame
{
	GENERATED_BODY()
public:
	int32 GetLevelOfAnUpgrade(const EUpgrade InUpgrade);
	
	void AddUpgrade(const EUpgrade InUpgrade, const int32 InLevel);
private:
	UPROPERTY()
	TMap<EUpgrade, int32> UnlockedUpgrades;
};
