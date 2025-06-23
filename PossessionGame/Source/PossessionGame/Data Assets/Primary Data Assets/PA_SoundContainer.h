#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PossessionGame/Utilities/Data Utilities/FTFunctionCallback.h"
#include "PA_SoundContainer.generated.h"

UCLASS()
class POSSESSIONGAME_API UPA_SoundContainer : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Sound Data")
	FName SoundName;
	UPROPERTY(EditAnywhere, Category = "Sound Data")
	TSoftObjectPtr<USoundBase> Sound;
	UPROPERTY(EditDefaultsOnly, Category = "Sound Data")
	bool bIsCharacterTypeDependant = false;
	UPROPERTY(EditDefaultsOnly, Category = "Sound Data", meta = (EditCondition = "bIsCharacterTypeDependant", EditConditionHides = true))
	float PlayerVolume = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Sound Data", meta = (EditCondition = "bIsCharacterTypeDependant", EditConditionHides = true))
	float EnemyVolume = 1.f;
	
	bool GetIsInLoad() const { return bIsInLoad; }
	bool GetIsLoaded() const { return bIsLoaded; }
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
	
	void SetIsInLoad(const bool bState) { bIsInLoad = bState; }
	void SetIsLoaded(const bool bState) { bIsLoaded = bState; }
	void AddSoundLoadedCallback(const TFunction<void()>& InCallback) { SoundsLoadedCallbacks.Add(FTFunctionCallback(InCallback)); }
	void TriggerSoundsCallbacks();
private:
	bool bIsInLoad = false;
	bool bIsLoaded = false;
	UPROPERTY()
	TArray<FTFunctionCallback> SoundsLoadedCallbacks;
};
