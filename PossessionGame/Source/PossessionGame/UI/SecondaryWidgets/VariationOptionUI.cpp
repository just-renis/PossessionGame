#include "VariationOptionUI.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

void UVariationOptionUI::UpdateUnlockProgressText(const FString& InText) { UnlockProgressText = DefaultUnlockText + InText; }

void UVariationOptionUI::UnlockVariation()
{
	bIsLocked = false;
	if (LockedImage) LockedImage->SetVisibility(ESlateVisibility::Collapsed);
}

void UVariationOptionUI::SetSelected(const bool bState)
{
	if (!SelectedText) return;
	bIsSelected = bState;
	SelectedText->SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
}

void UVariationOptionUI::SetButtonState(const bool bState) const
{
	if (!OptionButton) return;
	OptionButton->SetVisibility(bState ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void UVariationOptionUI::SelectVariation()
{
	if (bIsLocked)
	{
		GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::NotSelectable);
		return;
	}
	if (!SelectedText || bIsSelected) return;
	PlayAnimation(SelectAnim);
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::ButtonSelect);
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnVariationSelection.Broadcast(CharacterType, Variation);
	(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->OnVariationSelectionEnd.ExecuteIfBound(CharacterType);
	SetSelected(true);
}

FReply UVariationOptionUI::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton) return FReply::Handled();
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UVariationOptionUI::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (!VariationOptionBorder || !VariationOptionIcon) return;
	if (!bIsLocked && LockedImage) LockedImage->SetVisibility(ESlateVisibility::Collapsed);
	FSlateBrush Brush;
	Brush.SetResourceObject(VariationOptionIcon);
	VariationOptionBorder->SetBrush(Brush);
}

void UVariationOptionUI::NativeConstruct()
{
	Super::NativeConstruct();
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnVariationSelection.AddUObject(this, &UVariationOptionUI::SelectionChange);
	if (!OptionButton) return;
	OptionButton->OnClicked.AddDynamic(this, &UVariationOptionUI::SelectVariation);
	OptionButton->OnHovered.AddDynamic(this, &UVariationOptionUI::VariationOptionHovered);
	OptionButton->OnUnhovered.AddDynamic(this, &UVariationOptionUI::VariationOptionUnHovered);
}

void UVariationOptionUI::SelectionChange(const ECharacterType InCharacterType, const EVariation InVariationName)
{
	if (InCharacterType != CharacterType || Variation == InVariationName) return;
	SetSelected(false);
}

void UVariationOptionUI::VariationOptionHovered()
{
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::ButtonHover);
	if (!VariationOptionBorder) return;
	(void)ShowDetailsPanel.ExecuteIfBound(true, bIsLocked ? UnlockProgressText : DetailsPanelText);
	PlayAnimation(HoverAnim, GetAnimationCurrentTime(HoverAnim), 1, EUMGSequencePlayMode::Forward);
}

void UVariationOptionUI::VariationOptionUnHovered()
{
	if (!VariationOptionBorder) return;
	(void)ShowDetailsPanel.ExecuteIfBound(false, "");
	PlayAnimation(HoverAnim, GetAnimationCurrentTime(HoverAnim), 1, EUMGSequencePlayMode::Reverse);
}
