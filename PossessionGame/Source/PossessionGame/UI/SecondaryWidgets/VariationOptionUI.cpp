#include "VariationOptionUI.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

void UVariationOptionUI::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (!VariationOptionBorder || !VariationOptionIcon) return;
	if (!bIsLocked && LockedImage) LockedImage->SetVisibility(ESlateVisibility::Collapsed);
	FSlateBrush Brush;
	Brush.SetResourceObject(VariationOptionIcon);
	VariationOptionBorder->SetBrush(Brush);
}

void UVariationOptionUI::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::UpgradeCardHover);
	if (VariationOptionBorder) (void)ShowDetailsPanel.ExecuteIfBound(true, bIsLocked ? UnlockText : DetailsPanelText);
}

void UVariationOptionUI::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	if (VariationOptionBorder) (void)ShowDetailsPanel.ExecuteIfBound(false, FText());
}
