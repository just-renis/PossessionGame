#include "MainMenuButtonUI.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UMainMenuButtonUI::SetupEvents()
{
	Button->OnHovered.AddDynamic(this, &UMainMenuButtonUI::TriggerHoverAnim);
	Button->OnUnhovered.AddDynamic(this, &UMainMenuButtonUI::TriggerHoverAnim);
}

void UMainMenuButtonUI::TriggerHoverAnim()
{
	ButtonHoverState ? PlayAnimation(ButtonHoverAnim, GetAnimationCurrentTime(ButtonHoverAnim), 1, EUMGSequencePlayMode::Reverse) : PlayAnimation(ButtonHoverAnim, GetAnimationCurrentTime(ButtonHoverAnim));
	ButtonHoverState = !ButtonHoverState;
}

void UMainMenuButtonUI::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (ButtonText) ButtonText->SetText(Text);
}
