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
}

void UVariationOptionUI::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::UpgradeCardHover);
	if (!VariationOptionBorder) return;
	(void)ShowDetailsPanel.ExecuteIfBound(true, bIsLocked ? UnlockProgressText : DetailsPanelText);
	if (!HoverAnim) return;
	PlayAnimation(HoverAnim, GetAnimationCurrentTime(HoverAnim), 1, EUMGSequencePlayMode::Forward);
}

void UVariationOptionUI::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	if (!VariationOptionBorder) return;
	(void)ShowDetailsPanel.ExecuteIfBound(false, "");
	if (!HoverAnim) return;
	PlayAnimation(HoverAnim, GetAnimationCurrentTime(HoverAnim), 1, EUMGSequencePlayMode::Reverse);
}

void UVariationOptionUI::SelectVariation()
{
	if (!SelectAnim) return;
	PlayAnimation(SelectAnim);
}
