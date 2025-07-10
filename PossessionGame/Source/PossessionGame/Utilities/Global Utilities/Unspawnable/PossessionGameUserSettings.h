#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "PossessionGameUserSettings.generated.h"

class UPossessionGameInstance;

UCLASS()
class POSSESSIONGAME_API UPossessionGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()
public:
	float GetGamma() const { return GammaValue; }
	
	void SetGamma(const float InGammaValue);
	virtual void LoadSettings(const bool bForceReload) override;
private:
	UPROPERTY(Config)
	float GammaValue = 2.2f;
};
