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
	
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
