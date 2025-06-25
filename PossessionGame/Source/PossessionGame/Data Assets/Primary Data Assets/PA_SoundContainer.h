#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
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
	
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
