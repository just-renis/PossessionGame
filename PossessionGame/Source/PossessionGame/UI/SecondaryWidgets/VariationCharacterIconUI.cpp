#include "VariationCharacterIconUI.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

void UVariationCharacterIconUI::SetEventsEnabled(const bool bState)
{
	bEventsEnabled = bState;
	PlayUnHoverAnim();
}

void UVariationCharacterIconUI::PlayHoverAnim()
{
	if (bEventsEnabled)
	{
		GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::ButtonHover);
		PlayAnimation(HoverAnim, GetAnimationCurrentTime(HoverAnim), 1, EUMGSequencePlayMode::Forward);
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
	if (!bEventsEnabled) return;
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::PanelFadeIn);
	(void)OnVariationClicked.ExecuteIfBound(CharacterName);
	PlayAnimation(SelectAnim);
	SetEventsEnabled(false);
}

void UVariationCharacterIconUI::PlayUnHoverAnim()
{
	if (bEventsEnabled) PlayAnimation(HoverAnim, GetAnimationCurrentTime(HoverAnim), 1, EUMGSequencePlayMode::Reverse);
}
