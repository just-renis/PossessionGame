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
	bool GetMotionBlur() const { return bIsMotionBlurEnabled; }
	float GetGamma() const { return GammaValue; }
	float GetAntiAliasingMethod() const { return AntiAliasingMethod; }
	
	void SetMotionBlur(const bool bState);
	void SetGamma(const float InGammaValue);
	void SetAntiAliasingMethod(const int32 InAntiAliasingMethod);
	virtual void LoadSettings(const bool bForceReload) override;
private:
	UPROPERTY(Config)
	bool bIsMotionBlurEnabled = true;
	UPROPERTY(Config)
	int32 AntiAliasingMethod = 4;
	UPROPERTY(Config)
	float GammaValue = 2.2f;
};
