#include "ArrowButtonUI.h"
#include "Components/Button.h"

void UArrowButtonUI::SetupEvents()
{
	ArrowButton->OnHovered.AddDynamic(this, &UArrowButtonUI::PlayHoveredAnim);
	ArrowButton->OnUnhovered.AddDynamic(this, &UArrowButtonUI::PlayUnHoveredAnim);
	ArrowButton->OnPressed.AddDynamic(this, &UArrowButtonUI::PlayPressAnim);
}

void UArrowButtonUI::PlayHoveredAnim() { PlayAnimation(HoverAnim, GetAnimationCurrentTime(HoverAnim)); }

void UArrowButtonUI::PlayPressAnim() { PlayAnimation(PressAnim); }

void UArrowButtonUI::PlayUnHoveredAnim() { PlayAnimation(HoverAnim, GetAnimationCurrentTime(HoverAnim), 1, EUMGSequencePlayMode::Reverse); }
