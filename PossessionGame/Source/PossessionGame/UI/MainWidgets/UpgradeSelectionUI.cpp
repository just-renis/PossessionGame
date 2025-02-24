#include "UpgradeSelectionUI.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Enums/EWidget.h"
#include "PossessionGame/UI/SecondaryWidgets/UpgradeCardUI.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void UUpgradeSelectionUI::OpenUI()
{
	Toggle(true);
	PlayerCardOption1->SetCardBackgroundBorderImage(PlayerCardBackgroundTexture);
	PlayerCardOption2->SetCardBackgroundBorderImage(PlayerCardBackgroundTexture);
	EnemyCardOption1->SetCardBackgroundBorderImage(EnemyCardBackgroundTexture);
	EnemyCardOption2->SetCardBackgroundBorderImage(EnemyCardBackgroundTexture);
}

void UUpgradeSelectionUI::CloseUI() { Toggle(false); }

void UUpgradeSelectionUI::Toggle(const bool bState)
{
	UGameplayStatics::SetGamePaused(GetWorld(), bState);
	SetVisibility(bState ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	const TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->bShowMouseCursor = bState;
	if (bState) PlayerController->SetInputMode(FInputModeGameAndUI());
	else PlayerController->SetInputMode(FInputModeGameOnly());
	GetWorld()->GetSubsystem<UWidgetManager>()->SetCurrentStateOfUI(bState ? EWidget::UpgradeSelectionUI : EWidget::None);
}
