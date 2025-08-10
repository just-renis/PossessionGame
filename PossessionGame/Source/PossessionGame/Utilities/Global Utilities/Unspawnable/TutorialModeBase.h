#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TutorialModeBase.generated.h"

class AMarker;
class UEnvQuery;
enum class EGameMode : uint8;
class UGameModeData;
enum class EPostProcessMaterial : uint8;
enum class EUpgrade : uint8;
enum class EPoolObject : uint8;
class APoolObject;
class UUpgradeData;
enum class EWidget : uint8;
class USettingsSave;

UCLASS()
class POSSESSIONGAME_API ATutorialModeBase : public AGameModeBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Save Data", meta = (AllowPrivateAccess = true))
	TSubclassOf<USettingsSave> SettingsSaveClass;
	UPROPERTY(EditAnywhere, Category = "Classes", meta = (AllowPrivateAccess = true))
	TSubclassOf<AMarker> MarkerClass;
	UPROPERTY(EditAnywhere, Category = "EQS", meta = (AllowPrivateAccess = true))
	TObjectPtr<UEnvQuery> TargetEQS;
	UPROPERTY(EditAnywhere, Category = "Widget Manager", meta = (AllowPrivateAccess = true))
	TMap<EWidget, TSubclassOf<UUserWidget>> WidgetClasses;
	UPROPERTY(EditAnywhere, Category = "Game Modes Manager", meta = (AllowPrivateAccess = true))
	TMap<EGameMode, TObjectPtr<UGameModeData>> GameModesData;
	UPROPERTY(EditAnywhere, Category = "Post Process Manager", meta = (AllowPrivateAccess = true))
	TMap<EPostProcessMaterial, TObjectPtr<UMaterialInterface>> PostProcessMaterials;
	UPROPERTY(EditAnywhere, Category = "Upgrades", meta = (AllowPrivateAccess = true))
	TMap<EUpgrade, TObjectPtr<UUpgradeData>> Upgrades;
	UPROPERTY(EditAnywhere, Category = "Pooling Manager", meta = (AllowPrivateAccess = true))
	TMap<EPoolObject, TSubclassOf<APoolObject>> PoolObjects;
	UPROPERTY()
	FTimerHandle PSOPrecachingHandle;

	virtual void BeginPlay() override;
	void LoadSettingsData() const;
	void InitAfterPSOPrecache();
};
