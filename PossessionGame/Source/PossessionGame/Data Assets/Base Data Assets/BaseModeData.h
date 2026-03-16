#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BaseModeData.generated.h"

enum class EMarkerType : uint8;
class UUpgradeData;
enum class EUpgrade : uint8;
enum class EWidget : uint8;
class UThrowableData;
class APoolObject;
enum class EPoolObject : uint8;
enum class EPostProcessMaterial : uint8;
class UEnvQuery;
class AMarker;
class USettingsSave;
enum class ECharacterType : uint8;

UCLASS()
class POSSESSIONGAME_API UBaseModeData : public UDataAsset
{
	GENERATED_BODY()
public:
	bool GetIsDebugModeEnabled() const { return bToggleDebugMode; }
	ECharacterType GetCharacterToSpawn() const { return CharacterToSpawn; }
	TObjectPtr<UEnvQuery> GetEQS() { return TargetEQS; }
	TSubclassOf<USettingsSave>& GetSettingsSaveClass() { return SettingsSaveClass; }
	TSubclassOf<AMarker>& GetMarkerClass() { return MarkerClass; }
	TMap<EMarkerType, TObjectPtr<UTexture2D>>& GetMarkerTypes() { return MarkerTypes; }
	TMap<EWidget, TSubclassOf<UUserWidget>>& GetWidgetClasses() { return WidgetClasses; }
	TMap<EUpgrade, TObjectPtr<UUpgradeData>>& GetUpgrades() { return Upgrades; }
	TMap<EPoolObject, TSubclassOf<APoolObject>>& GetPoolObjects() { return PoolObjects; }
	TMap<EPoolObject, TObjectPtr<UThrowableData>>& GetThrowablesData() { return ThrowablesData; }
	TMap<EPostProcessMaterial, TObjectPtr<UMaterialInterface>>& GetPostProcessMaterials() { return PostProcessMaterials; }
private:
	UPROPERTY(EditDefaultsOnly, Category = "Debug", meta = (AllowPrivateAccess = true))
	bool bToggleDebugMode = false;
	UPROPERTY(EditDefaultsOnly, Category = "Character To Spawn", meta = (AllowPrivateAccess = true))
	ECharacterType CharacterToSpawn;
	UPROPERTY(EditDefaultsOnly, Category = "Save Data", meta = (AllowPrivateAccess = true))
	TSubclassOf<USettingsSave> SettingsSaveClass;
	UPROPERTY(EditDefaultsOnly, Category = "Markers", meta = (AllowPrivateAccess = true))
	TSubclassOf<AMarker> MarkerClass;
	UPROPERTY(EditDefaultsOnly, Category = "EQS", meta = (AllowPrivateAccess = true))
	TObjectPtr<UEnvQuery> TargetEQS;
	UPROPERTY(EditDefaultsOnly, Category = "Post Process Manager", meta = (AllowPrivateAccess = true))
	TMap<EPostProcessMaterial, TObjectPtr<UMaterialInterface>> PostProcessMaterials;
	UPROPERTY(EditDefaultsOnly, Category = "Pooling Manager", meta = (AllowPrivateAccess = true))
	TMap<EPoolObject, TSubclassOf<APoolObject>> PoolObjects;
	UPROPERTY(EditDefaultsOnly, Category = "Pooling Manager", meta = (AllowPrivateAccess = true))
	TMap<EPoolObject, TObjectPtr<UThrowableData>> ThrowablesData;
	UPROPERTY(EditDefaultsOnly, Category = "Widget Manager", meta = (AllowPrivateAccess = true))
	TMap<EWidget, TSubclassOf<UUserWidget>> WidgetClasses;
	UPROPERTY(EditDefaultsOnly, Category = "Upgrades", meta = (AllowPrivateAccess = true))
	TMap<EUpgrade, TObjectPtr<UUpgradeData>> Upgrades;
	UPROPERTY(EditDefaultsOnly, Category = "Markers", meta = (AllowPrivateAccess = true))
	TMap<EMarkerType, TObjectPtr<UTexture2D>> MarkerTypes;
};
