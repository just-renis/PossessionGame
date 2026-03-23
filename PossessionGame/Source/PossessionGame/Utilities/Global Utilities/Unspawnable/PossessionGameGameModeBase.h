#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PossessionGame/Utilities/Data Utilities/FPrefabCategoryData.h"
#include "PossessionGameGameModeBase.generated.h"

class UBaseModeData;
class UGameModeData;
enum class EGameMode : uint8;
enum class EPrefabCategory : uint8;

UCLASS()
class POSSESSIONGAME_API APossessionGameGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	bool GetIsDebugModeEnabled() const;
	bool GetIsArenaMode() const { return bIsArenaMode; }
	
	TArray<TSubclassOf<APrefab>>* GetPrefabsByCategory(const EPrefabCategory Category);
private:
	bool bIsArenaMode = false;
	float RunStartTime;
	FTimerHandle PSOPrecachingHandle;
	UPROPERTY(EditDefaultsOnly, Category = "Data", meta = (AllowPrivateAccess = true))
	TObjectPtr<UBaseModeData> BaseModeData;
	UPROPERTY(EditDefaultsOnly, Category = "Game Modes Manager", meta = (AllowPrivateAccess = true))
	bool bIsFinalChapter = false;
	UPROPERTY(EditAnywhere, Category = "Game Modes Manager", meta = (AllowPrivateAccess = true))
	TMap<EGameMode, TObjectPtr<UGameModeData>> GameModesData;
	UPROPERTY(EditAnywhere, Category = "Level Prefabs", meta = (AllowPrivateAccess = true))
	TMap<EPrefabCategory, FPrefabCategoryData> Prefabs;
	
	int32 GetRunFinishedTime() const;

	virtual void BeginPlay() override;
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	void LoadSettingsData();
	void SpawnArtifact() const;
	void InitAfterPSOPrecache();
	void SetRunStart();
};