#include "SettingsQualityUI.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "PossessionGame/Enums/EGraphicsQuality.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

EGraphicsQuality USettingsQualityUI::GetQualityEnum(uint8 Value)
{
	switch (Value)
	{
		case 0: return EGraphicsQuality::Low;
		case 1: return EGraphicsQuality::Medium;
		case 2: return EGraphicsQuality::High;
		case 3: return EGraphicsQuality::Epic;
		case 4: return EGraphicsQuality::Ultra;
		case 5: return EGraphicsQuality::Custom;
		default: return EGraphicsQuality::Low;
	}
}

void USettingsQualityUI::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (SettingQualityText)
	{
		FSlateFontInfo FontInfo = SettingQualityText->GetFont();
		FontInfo.Size = FontSize;
		SettingQualityText->SetFont(FontInfo);
	}
}

void USettingsQualityUI::NativeConstruct()
{
	Super::NativeConstruct();
	if (!IsGlobalGraphicsSetting) GetGameInstance()->GetSubsystem<UDelegateManager>()->OnGlobalGraphicsSettingChange.AddUObject(this, &USettingsQualityUI::UpdateQuality);
	else GetGameInstance()->GetSubsystem<UDelegateManager>()->OnGraphicsSettingChange.BindUObject(this, &USettingsQualityUI::SetGlobalGraphicsSettingToCustom);
	SetupButtonEvents();
}

void USettingsQualityUI::SetupButtonEvents()
{
	if (SettingButtonUpscale) SettingButtonUpscale->OnClicked.AddDynamic(this, &USettingsQualityUI::IncreaseQuality);
	if (SettingButtonDownscale) SettingButtonDownscale->OnClicked.AddDynamic(this, &USettingsQualityUI::LowerQuality);
}

EGraphicsQuality USettingsQualityUI::SetAndGetAppliedQuality()
{
	AppliedQuality = TemporalQuality;
	return AppliedQuality;
}

void USettingsQualityUI::UpdateQuality(uint8 QualityInt)
{
	TemporalQuality = GetQualityEnum(QualityInt);
	SettingQualityText->SetText(FText::FromString(UEnum::GetValueAsString(TemporalQuality).Replace(TEXT("EGraphicsQuality::"), TEXT(""))));
}

void USettingsQualityUI::UpdateDefaultQuality(uint8 QualityInt)
{
	AppliedQuality = GetQualityEnum(QualityInt);
	UpdateQuality(QualityInt);
}

void USettingsQualityUI::UpdateQuality()
{
	TemporalQuality = AppliedQuality;
	SettingQualityText->SetText(FText::FromString(UEnum::GetValueAsString(AppliedQuality).Replace(TEXT("EGraphicsQuality::"), TEXT(""))));
}

void USettingsQualityUI::LowerQuality()
{
	if (TemporalQuality != EGraphicsQuality::Low)
	{
		const uint8 QualityInt = static_cast<uint8>(TemporalQuality) - 1;
		UpdateQuality(QualityInt);
		if (!IsGlobalGraphicsSetting) GetGameInstance()->GetSubsystem<UDelegateManager>()->OnGraphicsSettingChange.ExecuteIfBound();
		else GetGameInstance()->GetSubsystem<UDelegateManager>()->OnGlobalGraphicsSettingChange.Broadcast(QualityInt);
	}
}

void USettingsQualityUI::IncreaseQuality()
{
	if (TemporalQuality != EGraphicsQuality::Ultra)
	{
		const uint8 QualityInt = static_cast<uint8>(TemporalQuality) + 1;
		UpdateQuality(QualityInt);
		if (!IsGlobalGraphicsSetting) GetGameInstance()->GetSubsystem<UDelegateManager>()->OnGraphicsSettingChange.ExecuteIfBound();
		else GetGameInstance()->GetSubsystem<UDelegateManager>()->OnGlobalGraphicsSettingChange.Broadcast(QualityInt);
	}
}
