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
	float GetGamma() { return GammaValue; }
	
	void SetGamma(float InGammaValue);
	virtual void LoadSettings(bool bForceReload = true) override;
private:
	UPROPERTY(Config)
	float GammaValue = 2.2f;
};
