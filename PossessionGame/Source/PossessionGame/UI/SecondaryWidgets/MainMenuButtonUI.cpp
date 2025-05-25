#include "MainMenuButtonUI.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UMainMenuButtonUI::SetupEvents()
{
	if (!Button) return;
	Button->OnHovered.AddDynamic(this, &UMainMenuButtonUI::TriggerHoverAnim);
	Button->OnUnhovered.AddDynamic(this, &UMainMenuButtonUI::TriggerUnHoverAnim);
}

void UMainMenuButtonUI::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (!ButtonText) return;
	ButtonText->SetText(Text);
	FSlateFontInfo FontInfo = ButtonText->GetFont();
	FontInfo.Size = FontSize;
	ButtonText->SetFont(FontInfo);
}

void UMainMenuButtonUI::TriggerHoverAnim() { if (ButtonHoverAnim) PlayAnimation(ButtonHoverAnim, GetAnimationCurrentTime(ButtonHoverAnim)); }

void UMainMenuButtonUI::TriggerUnHoverAnim() { if (ButtonHoverAnim) PlayAnimation(ButtonHoverAnim, GetAnimationCurrentTime(ButtonHoverAnim), 1, EUMGSequencePlayMode::Reverse); }