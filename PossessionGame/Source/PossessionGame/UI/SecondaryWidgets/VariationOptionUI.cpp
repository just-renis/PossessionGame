#include "VariationOptionUI.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

void UVariationOptionUI::UpdateUnlockProgressText(const FString& InText) { UnlockProgressText = DefaultUnlockText + InText; }

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
	if (!OptionButton) return;
	OptionButton->OnClicked.AddDynamic(this, &UVariationOptionUI::SelectVariation);
	OptionButton->OnHovered.AddDynamic(this, &UVariationOptionUI::VariationOptionHovered);
	OptionButton->OnUnhovered.AddDynamic(this, &UVariationOptionUI::VariationOptionUnHovered);
}

FReply UVariationOptionUI::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton) return FReply::Handled();
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UVariationOptionUI::SelectVariation()
{
	if (!SelectAnim || bIsLocked) return;
	PlayAnimation(SelectAnim);
}

void UVariationOptionUI::VariationOptionHovered()
{
	GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::UpgradeCardHover);
	if (!VariationOptionBorder) return;
	(void)ShowDetailsPanel.ExecuteIfBound(true, bIsLocked ? UnlockProgressText : DetailsPanelText);
	if (!HoverAnim) return;
	PlayAnimation(HoverAnim, GetAnimationCurrentTime(HoverAnim), 1, EUMGSequencePlayMode::Forward);
}

void UVariationOptionUI::VariationOptionUnHovered()
{
	if (!VariationOptionBorder) return;
	(void)ShowDetailsPanel.ExecuteIfBound(false, "");
	if (!HoverAnim) return;
	PlayAnimation(HoverAnim, GetAnimationCurrentTime(HoverAnim), 1, EUMGSequencePlayMode::Reverse);
}
