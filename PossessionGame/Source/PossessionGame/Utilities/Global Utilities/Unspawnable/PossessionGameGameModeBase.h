#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PossessionGame/Utilities/Data Utilities/FPrefabCategoryData.h"
#include "PossessionGameGameModeBase.generated.h"

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
	bool GetIsDebugModeEnabled() const { return bToggleDebugMode; }
	
	TArray<TSubclassOf<APrefab>>* GetPrefabsByCategory(const EPrefabCategory Category);
private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	bool bToggleDebugMode = false;
	UPROPERTY(EditAnywhere, Category = "Save Data", meta = (AllowPrivateAccess = true))
	TSubclassOf<USettingsSave> SettingsSaveClass;
	UPROPERTY(EditAnywhere, Category = "EQS", meta = (AllowPrivateAccess = true))
	TObjectPtr<UEnvQuery> TargetEQS;
	UPROPERTY(EditAnywhere, Category = "Game Modes Manager", meta = (AllowPrivateAccess = true))
	TMap<EGameMode, TObjectPtr<UGameModeData>> GameModesData;
	UPROPERTY(EditAnywhere, Category = "Level Prefabs", meta = (AllowPrivateAccess = true))
	TMap<EPrefabCategory, FPrefabCategoryData> Prefabs;
	UPROPERTY(EditAnywhere, Category = "Post Process Manager", meta = (AllowPrivateAccess = true))
	TMap<EPostProcessMaterial, TObjectPtr<UMaterialInterface>> PostProcessMaterials;
	UPROPERTY(EditAnywhere, Category = "Pooling Manager", meta = (AllowPrivateAccess = true))
	TMap<EPoolObject, TSubclassOf<APoolObject>> PoolObjects;
	UPROPERTY(EditAnywhere, Category = "Widget Manager", meta = (AllowPrivateAccess = true))
	TMap<EWidget, TSubclassOf<UUserWidget>> WidgetClasses;
	UPROPERTY(EditAnywhere, Category = "Upgrades", meta = (AllowPrivateAccess = true))
	TMap<EUpgrade, TObjectPtr<UUpgradeData>> Upgrades;
//	UPROPERTY()
//	FTimerHandle FCutsceneTimerHandle;

	virtual void BeginPlay() override;
	void LoadSettingsData();
	void SpawnArtifact() const;
//	void PlayCutscene(const EGameMode InGameMode);
//	void CutsceneEnded() const;
};