#include "SettingsOptionUI.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void USettingsOptionUI::SetText(const FText& InText) const { if (Text) Text->SetText(InText); }

void USettingsOptionUI::NativeConstruct()
{
	Super::NativeConstruct();
	SetupButtonEvents();
}

void USettingsOptionUI::OnLeftArrowClicked() { if (LeftArrowClickAnim) PlayAnimation(LeftArrowClickAnim); }

void USettingsOptionUI::OnRightArrowClicked() { if (RightArrowClickAnim) PlayAnimation(RightArrowClickAnim); }

void USettingsOptionUI::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (Text)
	{
		FSlateFontInfo FontInfo = Text->GetFont();
		FontInfo.Size = FontSize;
		Text->SetFont(FontInfo);
	}
}

void USettingsOptionUI::SetupButtonEvents()
{
	if (UpscaleButton)
	{
		UpscaleButton->OnClicked.AddDynamic(this, &USettingsOptionUI::OnRightArrowClicked);
		UpscaleButton->OnHovered.AddDynamic(this, &USettingsOptionUI::OnRightArrowHovered);
		UpscaleButton->OnUnhovered.AddDynamic(this, &USettingsOptionUI::OnRightArrowUnHovered);
	}
	if (DownscaleButton)
	{
		DownscaleButton->OnClicked.AddDynamic(this, &USettingsOptionUI::OnLeftArrowClicked);
		DownscaleButton->OnHovered.AddDynamic(this, &USettingsOptionUI::OnLeftArrowHovered);
		DownscaleButton->OnUnhovered.AddDynamic(this, &USettingsOptionUI::OnLeftArrowUnHovered);
	}
}

void USettingsOptionUI::OnLeftArrowHovered()
{
	if (!LeftArrowHoverAnim) return;
	PlayAnimation(LeftArrowHoverAnim, GetAnimationCurrentTime(LeftArrowHoverAnim), 1, EUMGSequencePlayMode::Forward);
}

void USettingsOptionUI::OnLeftArrowUnHovered()
{
	if (!LeftArrowHoverAnim) return;
	PlayAnimation(LeftArrowHoverAnim, GetAnimationCurrentTime(LeftArrowHoverAnim), 1, EUMGSequencePlayMode::Reverse);
}

void USettingsOptionUI::OnRightArrowHovered()
{
	if (!RightArrowHoverAnim) return;
	PlayAnimation(RightArrowHoverAnim, GetAnimationCurrentTime(RightArrowHoverAnim), 1, EUMGSequencePlayMode::Forward);
}

void USettingsOptionUI::OnRightArrowUnHovered()
{
	if (!RightArrowHoverAnim) return;
	PlayAnimation(RightArrowHoverAnim, GetAnimationCurrentTime(RightArrowHoverAnim), 1, EUMGSequencePlayMode::Reverse);
}
