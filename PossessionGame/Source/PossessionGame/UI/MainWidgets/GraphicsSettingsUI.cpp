#include "GraphicsSettingsUI.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/UI/SecondaryWidgets/SettingsOptionUI.h"
#include "PossessionGame/UI/SecondaryWidgets/SettingsQualityUI.h"
#include "PossessionGame/UI/SecondaryWidgets/SliderUI.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameUserSettings.h"

void UGraphicsSettingsUI::SetupUIEvents()
{
	if (ScreenModeOption)
	{
		ScreenModeOption->GetDownscaleButton()->OnClicked.AddDynamic(this, &UGraphicsSettingsUI::LowerWindowMode);
		ScreenModeOption->GetUpscaleButton()->OnClicked.AddDynamic(this, &UGraphicsSettingsUI::IncreaseWindowMode);
	}
	if (ResolutionOption)
	{
		ResolutionOption->GetDownscaleButton()->OnClicked.AddDynamic(this, &UGraphicsSettingsUI::LowerResolution);
		ResolutionOption->GetUpscaleButton()->OnClicked.AddDynamic(this, &UGraphicsSettingsUI::IncreaseResolution);
	}
	if (VsyncOption)
	{
		VsyncOption->GetDownscaleButton()->OnClicked.AddDynamic(this, &UGraphicsSettingsUI::DisableVsync);
		VsyncOption->GetUpscaleButton()->OnClicked.AddDynamic(this, &UGraphicsSettingsUI::EnableVsync);
	}
	if (BrightnessSlider) BrightnessSlider->GetSlider()->OnValueChanged.AddDynamic(this, &UGraphicsSettingsUI::ApplyBrightnessValue);
}

void UGraphicsSettingsUI::UpdateDefaultGraphicsSettings()
{
	const TObjectPtr<UPossessionGameUserSettings> GameUserSettings = Cast<UPossessionGameUserSettings>(GEngine->GetGameUserSettings());
	if (GlobalGraphics) GlobalGraphics->UpdateDefaultQuality(GetGlobalGraphicsQuality());
	if (AntiAliasing) AntiAliasing->UpdateDefaultQuality(GameUserSettings->GetAntiAliasingQuality());
	if (PostProcessing) PostProcessing->UpdateDefaultQuality(GameUserSettings->GetPostProcessingQuality());
	if (GlobalIllumination) GlobalIllumination->UpdateDefaultQuality(GameUserSettings->GetGlobalIlluminationQuality());
	if (Textures) Textures->UpdateDefaultQuality(GameUserSettings->GetTextureQuality());
	if (Effects) Effects->UpdateDefaultQuality(GameUserSettings->GetVisualEffectQuality());
	if (Shading) Shading->UpdateDefaultQuality(GameUserSettings->GetShadingQuality());
	if (ScreenMode) ScreenMode = GameUserSettings->GetFullscreenMode();
	if (ScreenModeOption) ScreenModeOption->SetText(FText::FromString(GetFixedScreenMode()));
	Resolution = GEngine->GetGameUserSettings()->GetDesktopResolution();
	if (ResolutionOption) ResolutionOption->SetText(FText::FromString(FString::Format(TEXT("{0}x{1}"), {Resolution.X, Resolution.Y})));
	Vsync = GameUserSettings->IsVSyncEnabled();
	if (VsyncOption) VsyncOption->SetText(FText::FromString(Vsync ? "Enabled" : "Disabled"));
	if (BrightnessSlider)
	{
		BrightnessSlider->SetValue((GameUserSettings->GetGamma() - 1.f) / 4.f);
		ApplyBrightnessValue(BrightnessSlider->GetSavedValue());
	}
}

void UGraphicsSettingsUI::SetSavedSettings()
{
	if (GlobalGraphics) GlobalGraphics->UpdateQuality();
	if (AntiAliasing) AntiAliasing->UpdateQuality();
	if (PostProcessing) PostProcessing->UpdateQuality();
	if (GlobalIllumination) GlobalIllumination->UpdateQuality();
	if (Textures) Textures->UpdateQuality();
	if (Effects) Effects->UpdateQuality();
	if (Shading) Shading->UpdateQuality();
	if (BrightnessSlider)
	{
		BrightnessSlider->SetSliderValue(BrightnessSlider->GetSavedValue());
		ApplyBrightnessValue(BrightnessSlider->GetSavedValue());
	}
}

void UGraphicsSettingsUI::ApplySettings()
{
	if (const TObjectPtr<UPossessionGameUserSettings> UserSettings = Cast<UPossessionGameUserSettings>(GEngine->GetGameUserSettings()))
	{
		if (AntiAliasing) UserSettings->SetAntiAliasingQuality(static_cast<uint8>(AntiAliasing->SetAndGetAppliedQuality()));
		if (PostProcessing) UserSettings->SetPostProcessingQuality(static_cast<uint8>(PostProcessing->SetAndGetAppliedQuality()));
		if (GlobalIllumination) UserSettings->SetGlobalIlluminationQuality(static_cast<uint8>(GlobalIllumination->SetAndGetAppliedQuality()));
		if (Textures) UserSettings->SetTextureQuality(static_cast<uint8>(Textures->SetAndGetAppliedQuality()));
		if (Effects) UserSettings->SetVisualEffectQuality(static_cast<uint8>(Effects->SetAndGetAppliedQuality()));
		if (Shading) UserSettings->SetShadingQuality(static_cast<uint8>(Shading->SetAndGetAppliedQuality()));
		if (GlobalGraphics)
		{
			GlobalGraphics->UpdateQuality(GetGlobalGraphicsQuality());
			GlobalGraphics->SetAndGetAppliedQuality();
		}
		UserSettings->SetFullscreenMode(ScreenMode);
		UserSettings->SetScreenResolution(Resolution);
		UserSettings->SetVSyncEnabled(Vsync);
		if (BrightnessSlider)
		{
			BrightnessSlider->SaveValue();
			UserSettings->SetGamma(BrightnessSlider->GetSavedValue() * 4.f + 1.f);
		}
		UserSettings->ApplySettings(true);
	}
}

uint8 UGraphicsSettingsUI::GetGlobalGraphicsQuality()
{
	const TObjectPtr<UPossessionGameUserSettings> GameUserSettings = Cast<UPossessionGameUserSettings>(GEngine->GetGameUserSettings());
	TArray QualitySettings = {
		GameUserSettings->GetGlobalIlluminationQuality(),
		GameUserSettings->GetAntiAliasingQuality(),
		GameUserSettings->GetPostProcessingQuality(),
		GameUserSettings->GetTextureQuality(),
		GameUserSettings->GetVisualEffectQuality(),
		GameUserSettings->GetShadingQuality()
	};
	const int32 Value = QualitySettings[0];
	for (int32 i = 1; i < QualitySettings.Num(); i++) if (Value != QualitySettings[i]) return 5;
	return Value;
}

FString UGraphicsSettingsUI::GetFixedScreenMode() const
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
	UKismetSystemLibrary::GetSupportedFullscreenResolutions(SupportedResolutions);
	if (SupportedResolutions.IsEmpty()) SupportedResolutions.Add(FIntPoint(1920, 1080));
	ResolutionIndex = SupportedResolutions.IndexOfByPredicate([&](const FIntPoint& Resolution)
	{
		return Resolution == GEngine->GetGameUserSettings()->GetDesktopResolution();
	});
	if (ResolutionIndex == -1) ResolutionIndex = SupportedResolutions.Num() - 1;
}

void UGraphicsSettingsUI::ApplyBrightnessValue(const float InValue)
{
	Cast<UPossessionGameUserSettings>(GEngine->GetGameUserSettings())->SetGamma(InValue * 4 + 1.f);
}

void UGraphicsSettingsUI::LowerWindowMode()
{
	if (ScreenMode != EWindowMode::Windowed)
	{
		ScreenMode = static_cast<EWindowMode::Type>(static_cast<uint8>(ScreenMode) + 1);
		if (ScreenModeOption) ScreenModeOption->SetText(FText::FromString(GetFixedScreenMode()));
	}
}

void UGraphicsSettingsUI::IncreaseWindowMode()
{
	if (ScreenMode != EWindowMode::Fullscreen)
	{
		ScreenMode = static_cast<EWindowMode::Type>(static_cast<uint8>(ScreenMode) - 1);
		if (ScreenModeOption) ScreenModeOption->SetText(FText::FromString(GetFixedScreenMode()));
	}
}

void UGraphicsSettingsUI::LowerResolution()
{
	if (ResolutionIndex > 0)
	{
		Resolution = SupportedResolutions[--ResolutionIndex];
		if (ResolutionOption) ResolutionOption->SetText(FText::FromString(FString::Format(TEXT("{0}x{1}"), {Resolution.X, Resolution.Y})));
	}
}

void UGraphicsSettingsUI::IncreaseResolution()
{
	if (ResolutionIndex < SupportedResolutions.Num() - 1)
	{
		Resolution = SupportedResolutions[++ResolutionIndex];
		if (ResolutionOption) ResolutionOption->SetText(FText::FromString(FString::Format(TEXT("{0}x{1}"), {Resolution.X, Resolution.Y})));
	}
}

void UGraphicsSettingsUI::DisableVsync()
{
	Vsync = false;
	if (VsyncOption) VsyncOption->SetText(FText::FromString("Disabled"));
}

void UGraphicsSettingsUI::EnableVsync()
{
	Vsync = true;
	if (VsyncOption) VsyncOption->SetText(FText::FromString("Enabled"));
}
