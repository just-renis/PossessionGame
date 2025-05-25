#include "SettingsQualityUI.h"
#include "Components/TextBlock.h"
#include "PossessionGame/Enums/EGraphicsQuality.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

EGraphicsQuality USettingsQualityUI::GetQualityEnum(const uint8 InValue)
{
	switch (InValue)
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

void USettingsQualityUI::NativeConstruct()
{
	Super::NativeConstruct();
	if (!IsGlobalGraphicsSetting) GetGameInstance()->GetSubsystem<UDelegateManager>()->OnGlobalGraphicsSettingChange.AddUObject(this, &USettingsQualityUI::UpdateQuality);
	else GetGameInstance()->GetSubsystem<UDelegateManager>()->OnGraphicsSettingChange.BindUObject(this, &USettingsQualityUI::SetGlobalGraphicsSettingToCustom);
}

void USettingsQualityUI::OnLeftArrowClicked()
{
	Super::OnLeftArrowClicked();
	if (TemporalQuality != EGraphicsQuality::Low)
	{
		const uint8 QualityInt = static_cast<uint8>(TemporalQuality) - 1;
		UpdateQuality(QualityInt);
		if (!IsGlobalGraphicsSetting) (void)GetGameInstance()->GetSubsystem<UDelegateManager>()->OnGraphicsSettingChange.ExecuteIfBound();
		else GetGameInstance()->GetSubsystem<UDelegateManager>()->OnGlobalGraphicsSettingChange.Broadcast(QualityInt);
	}
}

void USettingsQualityUI::OnRightArrowClicked()
{
	Super::OnRightArrowClicked();
	if (TemporalQuality != EGraphicsQuality::Ultra)
	{
		const uint8 QualityInt = static_cast<uint8>(TemporalQuality) + 1;
		UpdateQuality(QualityInt);
		if (!IsGlobalGraphicsSetting) (void)GetGameInstance()->GetSubsystem<UDelegateManager>()->OnGraphicsSettingChange.ExecuteIfBound();
		else GetGameInstance()->GetSubsystem<UDelegateManager>()->OnGlobalGraphicsSettingChange.Broadcast(QualityInt);
	}
}

EGraphicsQuality USettingsQualityUI::SetAndGetAppliedQuality()
{
	AppliedQuality = TemporalQuality;
	return AppliedQuality;
}

void USettingsQualityUI::UpdateQuality(const uint8 InQualityInt)
{
	TemporalQuality = GetQualityEnum(InQualityInt);
	if (Text) Text->SetText(FText::FromString(UEnum::GetValueAsString(TemporalQuality).Replace(TEXT("EGraphicsQuality::"), TEXT(""))));
}

void USettingsQualityUI::UpdateDefaultQuality(const uint8 InQualityInt)
{
	AppliedQuality = GetQualityEnum(InQualityInt);
	UpdateQuality(InQualityInt);
}

void USettingsQualityUI::UpdateQuality()
{
	TemporalQuality = AppliedQuality;
	if (Text) Text->SetText(FText::FromString(UEnum::GetValueAsString(AppliedQuality).Replace(TEXT("EGraphicsQuality::"), TEXT(""))));
}
