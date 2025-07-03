#include "VariationCharacterIconUI.h"
#include "Components/Border.h"
#include "Components/Button.h"

void UVariationCharacterIconUI::SetEventsEnabled(const bool bState)
{
	bEventsEnabled = bState;
	PlayUnHoverAnim();
}

void UVariationCharacterIconUI::SetButtonState(const bool bState) const
{
	if (!VariationCharacterButton) return;
	VariationCharacterButton->SetVisibility(bState ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void UVariationCharacterIconUI::PlayHoverAnim() { if (bEventsEnabled) PlayAnimation(HoverAnim, GetAnimationCurrentTime(HoverAnim), 1, EUMGSequencePlayMode::Forward); }

void UVariationCharacterIconUI::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (VariationCharacterBorder && VariationCharacterIcon)
	{
		FSlateBrush Brush;
		Brush.SetResourceObject(VariationCharacterIcon);
		VariationCharacterBorder->SetBrush(Brush);
	}
}

void UVariationCharacterIconUI::NativeConstruct()
{
	Super::NativeConstruct();
	if (!VariationCharacterButton) return;
	VariationCharacterButton->OnClicked.AddDynamic(this, &UVariationCharacterIconUI::HandleClicked);
	VariationCharacterButton->OnHovered.AddDynamic(this, &UVariationCharacterIconUI::PlayHoverAnim);
	VariationCharacterButton->OnUnhovered.AddDynamic(this, &UVariationCharacterIconUI::PlayUnHoverAnim);
}

void UVariationCharacterIconUI::PlaySelectedAnim() { PlayAnimation(SelectAnim); }

void UVariationCharacterIconUI::HandleClicked()
{
	if (!bEventsEnabled) return;
	(void)OnVariationClicked.ExecuteIfBound(VariationIndex);
	PlayAnimation(SelectAnim);
	SetEventsEnabled(false);
}

void UVariationCharacterIconUI::PlayUnHoverAnim() { if (bEventsEnabled) PlayAnimation(HoverAnim, GetAnimationCurrentTime(HoverAnim), 1, EUMGSequencePlayMode::Reverse); }
