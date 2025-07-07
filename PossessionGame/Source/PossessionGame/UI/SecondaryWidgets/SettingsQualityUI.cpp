#include "SettingsQualityUI.h"
#include "Components/TextBlock.h"
#include "PossessionGame/Enums/EGraphicsQuality.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

EGraphicsQuality USettingsQualityUI::GetQualityEnum(const uint8 InValue)
{
	EGraphicsQuality Quality;
	switch (InValue)
	{
		case 0: Quality = EGraphicsQuality::Low; break;
		case 1: Quality = EGraphicsQuality::Medium; break;
		case 2: Quality = EGraphicsQuality::High; break;
		case 3: Quality = EGraphicsQuality::Epic; break;
		case 4: Quality = EGraphicsQuality::Ultra; break;
		case 5: Quality = EGraphicsQuality::Custom; break;
		default: Quality = EGraphicsQuality::Low;
	}
	if (IsGlobalGraphicsSetting) return Quality;
	if (Quality > Cap) Quality = Cap;
	return Quality;
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
	if (TemporalQuality != Cap)
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
