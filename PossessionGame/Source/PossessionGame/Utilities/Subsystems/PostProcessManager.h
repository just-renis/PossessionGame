#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Enums/EPostProcessMaterial.h"
#include "Subsystems/WorldSubsystem.h"
#include "PostProcessManager.generated.h"

enum class EPostProcessMaterial : uint8;

UCLASS()
class POSSESSIONGAME_API UPostProcessManager : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	void Init(const TMap<EPostProcessMaterial, TObjectPtr<UMaterialInterface>>& InPostProcessMaterials);
	void TriggerGlitchingMaterial(const float InDuration);
	void ToggleMaterial(const EPostProcessMaterial InMaterialName, const float InValue);
	void AddOrUpdateDynamicMaterial(const EPostProcessMaterial InMaterialName, const TObjectPtr<UMaterialInstanceDynamic>& InMaterial, const float InWeight);
	void FadeOutDeathMaterials();
private:
	float FadingOutDeathMaterialsCounter;
	UPROPERTY()
	TObjectPtr<APostProcessVolume> PostProcessVolume;
	UPROPERTY()
	TMap<EPostProcessMaterial, TObjectPtr<UMaterialInterface>> Materials;

	UFUNCTION()
	void DisableGlitchingMaterial();
	UFUNCTION()
	void ProcessFadingOfDeathMaterials();
};
