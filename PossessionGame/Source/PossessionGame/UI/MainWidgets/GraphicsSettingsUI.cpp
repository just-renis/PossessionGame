#include "GraphicsSettingsUI.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "PossessionGame/UI/SecondaryWidgets/SettingsQualityUI.h"
#include "PossessionGame/UI/SecondaryWidgets/SliderUI.h"
#include "PossessionGame/Utilities/Global Utilities/PossessionGameUserSettings.h"

void UGraphicsSettingsUI::SetupUIEvents()
{
	if (ScreenModeButtonDownscale) ScreenModeButtonDownscale->OnClicked.AddDynamic(this, &UGraphicsSettingsUI::LowerWindowMode);
	if (ScreenModeButtonUpscale) ScreenModeButtonUpscale->OnClicked.AddDynamic(this, &UGraphicsSettingsUI::IncreaseWindowMode);
	if (ResolutionButtonDownscale) ResolutionButtonDownscale->OnClicked.AddDynamic(this, &UGraphicsSettingsUI::LowerResolution);
	if (ResolutionButtonUpscale) ResolutionButtonUpscale->OnClicked.AddDynamic(this, &UGraphicsSettingsUI::IncreaseResolution);
	if (VsyncButtonDisable) VsyncButtonDisable->OnClicked.AddDynamic(this, &UGraphicsSettingsUI::DisableVsync);
	if (VsyncButtonEnable) VsyncButtonEnable->OnClicked.AddDynamic(this, &UGraphicsSettingsUI::EnableVsync);
	if (BrightnessSlider) BrightnessSlider->GetSlider()->OnValueChanged.AddDynamic(this, &UGraphicsSettingsUI::ApplyBrightnessValue);
}

void UGraphicsSettingsUI::UpdateDefaultGraphicsSettings()
{
	TObjectPtr<UPossessionGameUserSettings> GameUserSettings = Cast<UPossessionGameUserSettings>(GEngine->GetGameUserSettings());
	GlobalGraphics->UpdateDefaultQuality(GetGlobalGraphicsQuality());
	AntiAliasing->UpdateDefaultQuality(GameUserSettings->GetAntiAliasingQuality());
	PostProcessing->UpdateDefaultQuality(GameUserSettings->GetPostProcessingQuality());
	GlobalIllumination->UpdateDefaultQuality(GameUserSettings->GetGlobalIlluminationQuality());
	Textures->UpdateDefaultQuality(GameUserSettings->GetTextureQuality());
	Effects->UpdateDefaultQuality(GameUserSettings->GetVisualEffectQuality());
	Shading->UpdateDefaultQuality(GameUserSettings->GetShadingQuality());
	ScreenMode = GameUserSettings->GetFullscreenMode();
	ScreenModeMode->SetText(FText::FromString(GetFixedScreenMode()));
	Resolution = GameUserSettings->GetScreenResolution();
	ResolutionMode->SetText(FText::FromString(FString::Printf(TEXT("%dx%d"), Resolution.X, Resolution.Y)));
	Vsync = GameUserSettings->IsVSyncEnabled();
	VsyncMode->SetText(FText::FromString(Vsync ? "Enabled" : "Disabled"));
	BrightnessSlider->SetValue((GameUserSettings->GetGamma() - 1.f) / 4.f);
	ApplyBrightnessValue(BrightnessSlider->GetSavedValue());
}

void UGraphicsSettingsUI::SetSavedSettings()
{
	GlobalGraphics->UpdateQuality();
	AntiAliasing->UpdateQuality();
	PostProcessing->UpdateQuality();
	GlobalIllumination->UpdateQuality();
	Textures->UpdateQuality();
	Effects->UpdateQuality();
	Shading->UpdateQuality();
	BrightnessSlider->SetSliderValue(BrightnessSlider->GetSavedValue());
	ApplyBrightnessValue(BrightnessSlider->GetSavedValue());
}

void UGraphicsSettingsUI::ApplySettings()
{
	if (TObjectPtr<UPossessionGameUserSettings> UserSettings = Cast<UPossessionGameUserSettings>(GEngine->GetGameUserSettings()))
	{
		UserSettings->SetAntiAliasingQuality(static_cast<uint8>(AntiAliasing->SetAndGetAppliedQuality()));
		UserSettings->SetPostProcessingQuality(static_cast<uint8>(PostProcessing->SetAndGetAppliedQuality()));
		UserSettings->SetGlobalIlluminationQuality(static_cast<uint8>(GlobalIllumination->SetAndGetAppliedQuality()));
		UserSettings->SetTextureQuality(static_cast<uint8>(Textures->SetAndGetAppliedQuality()));
		UserSettings->SetVisualEffectQuality(static_cast<uint8>(Effects->SetAndGetAppliedQuality()));
		UserSettings->SetShadingQuality(static_cast<uint8>(Shading->SetAndGetAppliedQuality()));
		GlobalGraphics->UpdateQuality(GetGlobalGraphicsQuality());
		GlobalGraphics->SetAndGetAppliedQuality();
		UserSettings->SetFullscreenMode(ScreenMode);
		UserSettings->SetScreenResolution(Resolution);
		UserSettings->SetVSyncEnabled(Vsync);
		BrightnessSlider->SaveValue();
		UserSettings->SetGamma(BrightnessSlider->GetSavedValue() * 4.f + 1.f);
		UserSettings->ApplySettings(true);
	}
}

uint8 UGraphicsSettingsUI::GetGlobalGraphicsQuality()
{
	TObjectPtr<UPossessionGameUserSettings> GameUserSettings = Cast<UPossessionGameUserSettings>(GEngine->GetGameUserSettings());
	TArray QualitySettings = {
		GameUserSettings->GetGlobalIlluminationQuality(),
		GameUserSettings->GetAntiAliasingQuality(),
		GameUserSettings->GetPostProcessingQuality(),
		GameUserSettings->GetTextureQuality(),
		GameUserSettings->GetVisualEffectQuality(),
		GameUserSettings->GetShadingQuality()
	};
	int32 Value = QualitySettings[0];
	for (int32 i = 1; i < QualitySettings.Num(); i++) if (Value != QualitySettings[i]) return 5;
	return Value;
}

FString UGraphicsSettingsUI::GetFixedScreenMode()
{
	FString WindowMode = UEnum::GetValueAsString(ScreenMode).Replace(TEXT("EWindowMode::"), TEXT(""));
	if (WindowMode.Equals("WindowedFullscreen")) WindowMode = "WindowFull";
	return WindowMode;
}

void UGraphicsSettingsUI::NativeConstruct()
{
	Super::NativeConstruct();
	InitResolutions();
}

void UGraphicsSettingsUI::InitResolutions()
{
	SupportedResolutions.Add(FIntPoint(800, 600));
	SupportedResolutions.Add(FIntPoint(1280, 720));
	SupportedResolutions.Add(FIntPoint(1440, 900));
	SupportedResolutions.Add(FIntPoint(1600, 900));
	SupportedResolutions.Add(FIntPoint(1920, 1080));
	ResolutionIndex = SupportedResolutions.IndexOfByPredicate([&](const FIntPoint& Resolution)
	{
		return Resolution == GEngine->GetGameUserSettings()->GetScreenResolution();
	});
	if (ResolutionIndex == INDEX_NONE) ResolutionIndex = 4;
}

void UGraphicsSettingsUI::ApplyBrightnessValue(float Value)
{
	Cast<UPossessionGameUserSettings>(GEngine->GetGameUserSettings())->SetGamma(Value * 4 + 1.f);
}

void UGraphicsSettingsUI::LowerWindowMode()
{
	if (ScreenMode != EWindowMode::Windowed)
	{
		ScreenMode = static_cast<EWindowMode::Type>(static_cast<uint8>(ScreenMode) + 1);
		ScreenModeMode->SetText(FText::FromString(GetFixedScreenMode()));
	}
}

void UGraphicsSettingsUI::IncreaseWindowMode()
{
	if (ScreenMode != EWindowMode::Fullscreen)
	{
		ScreenMode = static_cast<EWindowMode::Type>(static_cast<uint8>(ScreenMode) - 1);
		ScreenModeMode->SetText(FText::FromString(GetFixedScreenMode()));
	}
}

void UGraphicsSettingsUI::LowerResolution()
{
	if (ResolutionIndex > 0)
	{
		Resolution = SupportedResolutions[--ResolutionIndex];
		ResolutionMode->SetText(FText::FromString(FString::Printf(TEXT("%dx%d"), Resolution.X, Resolution.Y)));
	}
}

void UGraphicsSettingsUI::IncreaseResolution()
{
	if (ResolutionIndex < SupportedResolutions.Num() - 1)
	{
		Resolution = SupportedResolutions[++ResolutionIndex];
		ResolutionMode->SetText(FText::FromString(FString::Printf(TEXT("%dx%d"), Resolution.X, Resolution.Y)));
	}
}

void UGraphicsSettingsUI::DisableVsync()
{
	Vsync = false;
	VsyncMode->SetText(FText::FromString("Disabled"));
}

void UGraphicsSettingsUI::EnableVsync()
{
	Vsync = true;
	VsyncMode->SetText(FText::FromString("Enabled"));
}
