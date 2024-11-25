#include "SettingsTitleUI.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void USettingsTitleUI::SetTitleText(FText InTitleText) { if (Title) Title->SetText(InTitleText); }

void USettingsTitleUI::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (Title)
	{
		Title->SetText(TitleText);
		FSlateFontInfo FontInfo = Title->GetFont();
		FontInfo.Size = FontSize;
		Title->SetFont(FontInfo);
	}
	if (Decoration) Decoration->SetDesiredSizeOverride(ImageSize);
}
