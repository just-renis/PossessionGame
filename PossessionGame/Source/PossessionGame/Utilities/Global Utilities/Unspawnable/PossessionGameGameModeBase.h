#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PossessionGame/Data Assets/Base Data Assets/BaseModeData.h"
#include "PossessionGame/Utilities/Data Utilities/FPrefabCategoryData.h"
#include "PossessionGameGameModeBase.generated.h"

class UBaseModeData;
class UThrowableData;
class USaveSlot;
enum class ECharacterType : uint8;
enum class EMarkerType : uint8;
class AMarker;
class UGameModeData;
enum class EGameMode : uint8;
enum class EUpgrade : uint8;
class UUpgradeData;
enum class EPostProcessMaterial : uint8;
class UEnvQuery;
enum class EPrefabCategory : uint8;
class USettingsSave;
enum class EWidget : uint8;
class APoolObject;
enum class EPoolObject : uint8;

UCLASS()
class POSSESSIONGAME_API APossessionGameGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	bool GetIsDebugModeEnabled() const { return BaseModeData->GetIsDebugModeEnabled(); }
	bool GetIsArenaMode() const { return bIsArenaMode; }
	
	TArray<TSubclassOf<APrefab>>* GetPrefabsByCategory(const EPrefabCategory Category);
private:
	bool bIsArenaMode = false;
	float RunStartTime;
	FTimerHandle PSOPrecachingHandle;
	UPROPERTY(EditDefaultsOnly, Category = "Data", meta = (AllowPrivateAccess = true))
	TObjectPtr<UBaseModeData> BaseModeData;
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