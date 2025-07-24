#include "AbilityResetButtonUI.h"
#include "Components/Button.h"

void UAbilityResetButtonUI::NativeConstruct()
{
	Super::NativeConstruct();
	if (!AbilityResetButton) return;
	AbilityResetButton->OnPressed.AddDynamic(this, &UAbilityResetButtonUI::OnSelect);
	AbilityResetButton->OnHovered.AddDynamic(this, &UAbilityResetButtonUI::OnHovered);
	AbilityResetButton->OnUnhovered.AddDynamic(this, &UAbilityResetButtonUI::OnUnHovered);
}

void UAbilityResetButtonUI::OnHovered() { PlayAnimationForward(HoverAnim); }

void UAbilityResetButtonUI::OnUnHovered() { PlayAnimationReverse(HoverAnim); }

void UAbilityResetButtonUI::OnSelect() { PlayAnimationForward(SelectAnim); }
