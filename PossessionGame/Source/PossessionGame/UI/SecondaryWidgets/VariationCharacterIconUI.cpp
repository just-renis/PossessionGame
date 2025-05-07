#include "VariationCharacterIconUI.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Border.h"
#include "Components/Button.h"

void UVariationCharacterIconUI::SetEventsEnabled(const bool bState)
{
	bEventsEnabled = bState;
	if (bEventsEnabled && HoverAnim)
	{
		UE_LOG(LogTemp, Warning, TEXT("HOVER ANIMATION PLAYED: %s"), *HoverAnim->GetName());
		PlayAnimation(HoverAnim, 0, 1, EUMGSequencePlayMode::Reverse);
	}
}

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
	if (!SelectAnim || !bEventsEnabled) return;
	PlayAnimation(SelectAnim);
	SetEventsEnabled(false);
}

void UVariationCharacterIconUI::PlayHoverAnim() { if (bEventsEnabled && HoverAnim) PlayAnimation(HoverAnim, GetAnimationCurrentTime(HoverAnim), 1, EUMGSequencePlayMode::Forward); }

void UVariationCharacterIconUI::PlayUnHoverAnim() { if (bEventsEnabled && HoverAnim) PlayAnimation(HoverAnim, GetAnimationCurrentTime(HoverAnim), 1, EUMGSequencePlayMode::Reverse); }
