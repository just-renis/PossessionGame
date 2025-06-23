#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GraphicsSettingsUI.generated.h"

class USettingsOptionUI;
class USettingsQualityUI;
class UButton;
class UTextBlock;
class USliderUI;

UCLASS()
class POSSESSIONGAME_API UGraphicsSettingsUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetupUIEvents();
	void UpdateDefaultGraphicsSettings();
	void SetSavedSettings();
	void ApplySettings();
private:
	bool Vsync;
	int32 ResolutionIndex = -1;
	FIntPoint Resolution;
	EWindowMode::Type ScreenMode;
	TArray<FIntPoint> SupportedResolutions;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<USettingsQualityUI> GlobalGraphics;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<USettingsQualityUI> AntiAliasing;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<USettingsQualityUI> PostProcessing;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<USettingsQualityUI> GlobalIllumination;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<USettingsQualityUI> Textures;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<USettingsQualityUI> Effects;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<USettingsQualityUI> Shading;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<USettingsOptionUI> ScreenModeOption;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<USettingsOptionUI> ResolutionOption;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<USettingsOptionUI> VsyncOption;
	UPROPERTY(EditAnywhere, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<USliderUI> BrightnessSlider;

	uint8 GetGlobalGraphicsQuality();
	FString GetFixedScreenMode() const;

	virtual void NativeConstruct() override;
	void InitResolutions();
	UFUNCTION()
	void LowerWindowMode();
	UFUNCTION()
	void IncreaseWindowMode();
	UFUNCTION()
	void LowerResolution();
	UFUNCTION()
	void IncreaseResolution();
	UFUNCTION()
	void DisableVsync();
	UFUNCTION()
	void EnableVsync();
	UFUNCTION()
	void ApplyBrightnessValue(const float InValue);
};
