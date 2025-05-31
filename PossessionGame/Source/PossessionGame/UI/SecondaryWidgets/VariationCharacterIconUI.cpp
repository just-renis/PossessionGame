#include "VariationCharacterIconUI.h"
#include "Components/Border.h"
#include "Components/Button.h"

void UVariationCharacterIconUI::SetEventsEnabled(const bool bState)
{
	bEventsEnabled = bState;
	if (bEventsEnabled) PlayAnimation(HoverAnim, 0, 1, EUMGSequencePlayMode::Reverse);
}

void UVariationCharacterIconUI::PlaySelectedAnim() { PlayAnimation(SelectAnim); }

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

void UVariationCharacterIconUI::HandleClicked()
{
	(void)OnVariationClicked.ExecuteIfBound(VariationIndex);
	if (!bEventsEnabled) return;
	PlayAnimation(SelectAnim);
	SetEventsEnabled(false);
}

void UVariationCharacterIconUI::PlayHoverAnim() { if (bEventsEnabled) PlayAnimation(HoverAnim, GetAnimationCurrentTime(HoverAnim), 1, EUMGSequencePlayMode::Forward); }

void UVariationCharacterIconUI::PlayUnHoverAnim() { if (bEventsEnabled) PlayAnimation(HoverAnim, GetAnimationCurrentTime(HoverAnim), 1, EUMGSequencePlayMode::Reverse); }
