#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveSlot.generated.h"

class UPossessionGameInstance;
enum class EUpgrade : uint8;

UCLASS()
class POSSESSIONGAME_API USaveSlot : public USaveGame
{
	GENERATED_BODY()
public:
	int32 GetLevelOfAnUpgrade(const EUpgrade InUpgrade);
	
	void SetGameInstance(const TObjectPtr<UPossessionGameInstance>& InGameInstance) { GameInstance = InGameInstance; }
	void AddUpgrade(const EUpgrade InUpgrade, const int32 InLevel);
private:
	UPROPERTY()
	TObjectPtr<UPossessionGameInstance> GameInstance;
	UPROPERTY()
	TMap<EUpgrade, int32> UnlockedUpgrades;
};
