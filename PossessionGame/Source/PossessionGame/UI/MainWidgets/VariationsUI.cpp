#include "VariationsUI.h"
#include "Components/WidgetSwitcher.h"
#include "PossessionGame/UI/SecondaryWidgets/VariationCharacterIconUI.h"
#include "PossessionGame/UI/SecondaryWidgets/VariationOptionUI.h"
#include "PossessionGame/UI/SecondaryWidgets/VariationUnlockDetailsUI.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void UVariationsUI::OpenUI() { Toggle(true); }

void UVariationsUI::CloseUI() { Toggle(false); }

void UVariationsUI::NativeConstruct()
{
	Super::NativeConstruct();
	if (GuardianCharacterIcon) GuardianCharacterIcon->OnVariationClicked.BindUObject(this, &UVariationsUI::SwitchVariation);
	if (RangerCharacterIcon) RangerCharacterIcon->OnVariationClicked.BindUObject(this, &UVariationsUI::SwitchVariation);
	if (TricksterCharacterIcon) TricksterCharacterIcon->OnVariationClicked.BindUObject(this, &UVariationsUI::SwitchVariation);
	if (GuardianVariationDefault) GuardianVariationDefault->ShowDetailsPanel.BindUObject(this, &UVariationsUI::ToggleDetailsPanel);
	if (GuardianVariationGold) GuardianVariationGold->ShowDetailsPanel.BindUObject(this, &UVariationsUI::ToggleDetailsPanel);
	if (RangerVariationBlue) RangerVariationBlue->ShowDetailsPanel.BindUObject(this, &UVariationsUI::ToggleDetailsPanel);
	if (TricksterVariationPurple) TricksterVariationPurple->ShowDetailsPanel.BindUObject(this, &UVariationsUI::ToggleDetailsPanel);
}

void UVariationsUI::Toggle(const bool bState)
{
	SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	const TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->bShowMouseCursor = bState;
	if (bState)
	{
		PlayerController->SetInputMode(FInputModeUIOnly());
		PlayerController->FlushPressedKeys();
	}
	else PlayerController->SetInputMode(FInputModeGameOnly());
	GetWorld()->GetSubsystem<UWidgetManager>()->SetCurrentStateOfUI(bState ? EWidget::VariationsUI : EWidget::None);
}

void UVariationsUI::SwitchVariation(const int32 InIndex) const { VariationsSwitcher->SetActiveWidgetIndex(InIndex); }

void UVariationsUI::ToggleDetailsPanel(const bool bState, const FText& InDetailsText, const FText& InUnlockText) const
{
	if (!UnlockDetailsPanel) return;
	UnlockDetailsPanel->SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
	UnlockDetailsPanel->SetDetailsText(InDetailsText);
	UnlockDetailsPanel->SetUnlockText(InUnlockText);
}
