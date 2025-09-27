#include "DeathScreenUI.h"
#include "Components/Button.h"
#include "PossessionGame/UI/SecondaryWidgets/MainMenuButtonUI.h"
#include "PossessionGame/Utilities/Data Utilities/FLoadLevelData.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void UDeathScreenUI::OpenUI()
{
	Toggle(true);
}

void UDeathScreenUI::NativeConstruct()
{
	Super::NativeConstruct();
	ReturnToMainMenuButton->Get()->OnClicked.AddDynamic(this, &UDeathScreenUI::ReturnToMainMenu);
	RestartButton->Get()->OnClicked.AddDynamic(this, &UDeathScreenUI::Restart);
}

void UDeathScreenUI::Toggle(const bool bState)
{
	SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	const TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->bShowMouseCursor = bState;
	if (bState)
	{
		PlayerController->SetInputMode(FInputModeGameAndUI());
		ToggleButtons(true);
		PlayAnimation(FadeInAnim);
	}
	else
	{
		PlayerController->SetInputMode(FInputModeGameOnly());
		ToggleButtons(false);
		PlayAnimation(FadeOutAnim);
	}
	GetWorld()->GetSubsystem<UWidgetManager>()->SetCurrentStateOfUI(bState ? EWidget::DeathMenuUI : EWidget::None);
}

void UDeathScreenUI::ToggleButtons(const bool bState) const
{
	RestartButton->SetButtonVisibility(bState);
	ReturnToMainMenuButton->SetButtonVisibility(bState);
}

void UDeathScreenUI::ReturnToMainMenu()
{
	GetGameInstance()->GetSubsystem<UDelegateManager>()->LoadLevel.ExecuteIfBound(FLoadLevelData(true, false, true, FName("MainMenu")));
}

void UDeathScreenUI::Restart()
{
	GetGameInstance()->GetSubsystem<UDelegateManager>()->LoadLevel.ExecuteIfBound(FLoadLevelData(true, false, true, FName("Level_1")));
}
