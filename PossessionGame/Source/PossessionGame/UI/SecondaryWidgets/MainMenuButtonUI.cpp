#include "MainMenuButtonUI.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UMainMenuButtonUI::SetText(const FText& InText) const { if (ButtonText) ButtonText->SetText(InText); }

void UMainMenuButtonUI::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (!ButtonText) return;
	ButtonText->SetText(Text);
	FSlateFontInfo FontInfo = ButtonText->GetFont();
	FontInfo.Size = FontSize;
	ButtonText->SetFont(FontInfo);
}

void UMainMenuButtonUI::NativeConstruct()
{
	Super::NativeConstruct();
	SetupButtonEvents();
}

void UMainMenuButtonUI::SetupButtonEvents()
{
	if (!Button) return;
	Button->OnClicked.AddDynamic(this, &UMainMenuButtonUI::TriggerClickAnim);
	Button->OnHovered.AddDynamic(this, &UMainMenuButtonUI::TriggerHoverAnim);
	Button->OnUnhovered.AddDynamic(this, &UMainMenuButtonUI::TriggerUnHoverAnim);
}

void UMainMenuButtonUI::TriggerClickAnim() { if (ButtonClickAnim) PlayAnimation(ButtonClickAnim); }

void UMainMenuButtonUI::TriggerHoverAnim()
{
	if (!ButtonHoverAnim) return;
	PlayAnimation(ButtonHoverAnim, GetAnimationCurrentTime(ButtonHoverAnim), 1, EUMGSequencePlayMode::Forward);
}

void UMainMenuButtonUI::TriggerUnHoverAnim()
{
	if (!ButtonHoverAnim) return;
	PlayAnimation(ButtonHoverAnim, GetAnimationCurrentTime(ButtonHoverAnim), 1, EUMGSequencePlayMode::Reverse);
}