#include "AbilityResetButtonUI.h"
#include "Components/Button.h"

void UAbilityResetButtonUI::NativeConstruct()
{
	Super::NativeConstruct();
	if (!AbilityResetButton) return;
	AbilityResetButton->OnHovered.AddDynamic(this, &UAbilityResetButtonUI::OnHovered);
	AbilityResetButton->OnUnhovered.AddDynamic(this, &UAbilityResetButtonUI::OnUnHovered);
}

void UAbilityResetButtonUI::OnHovered() { PlayAnimationForward(HoverAnim); }

void UAbilityResetButtonUI::OnUnHovered() { PlayAnimationReverse(HoverAnim); }
