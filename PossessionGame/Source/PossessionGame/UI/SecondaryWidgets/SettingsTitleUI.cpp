#include "SettingsTitleUI.h"
#include "Components/TextBlock.h"

void USettingsTitleUI::SetTitleText(const FText& InTitleText) const { if (Title) Title->SetText(InTitleText); }

void USettingsTitleUI::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (!Title) return;
	Title->SetText(TitleText);
	FSlateFontInfo FontInfo = Title->GetFont();
	FontInfo.Size = FontSize;
	Title->SetFont(FontInfo);
}
