#include "MainMenuButtonUI.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UMainMenuButtonUI::SetupEvents()
{
	Button->OnHovered.AddDynamic(this, &UMainMenuButtonUI::TriggerHoverAnim);
	Button->OnUnhovered.AddDynamic(this, &UMainMenuButtonUI::TriggerUnHoverAnim);
}

void UMainMenuButtonUI::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (ButtonText) ButtonText->SetText(Text);
}

void UMainMenuButtonUI::TriggerHoverAnim() { PlayAnimation(ButtonHoverAnim, GetAnimationCurrentTime(ButtonHoverAnim)); }

void UMainMenuButtonUI::TriggerUnHoverAnim() { PlayAnimation(ButtonHoverAnim, GetAnimationCurrentTime(ButtonHoverAnim), 1, EUMGSequencePlayMode::Reverse); }