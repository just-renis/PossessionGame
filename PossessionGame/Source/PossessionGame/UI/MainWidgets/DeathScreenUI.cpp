#include "DeathScreenUI.h"
#include "Components/Button.h"
#include "PossessionGame/UI/SecondaryWidgets/MainMenuButtonUI.h"
#include "PossessionGame/Utilities/Data Utilities/FLoadLevelData.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void UDeathScreenUI::OpenUI() { Toggle(true); }

void UDeathScreenUI::NativeConstruct()
{
	Super::NativeConstruct();
	ReturnToMainMenuButton->Get()->OnClicked.AddDynamic(this, &UDeathScreenUI::ReturnToMainMenu);
	RestartButton->Get()->OnClicked.AddDynamic(this, &UDeathScreenUI::Restart);
	ReturnToMainMenuButton->Get()->OnHovered.AddDynamic(this, &UDeathScreenUI::PlayHoverSound);
	RestartButton->Get()->OnHovered.AddDynamic(this, &UDeathScreenUI::PlayHoverSound);
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
	if (bAlreadyClickedButton) return;
	PlaySound(ButtonClickSound);
	bAlreadyClickedButton = true;
	LevelNameToLoad = FName("MainMenu");
	FadeFinishedDelegate.BindDynamic(this, &UDeathScreenUI::FadeOutToNextLevel);
	PlayAnimation(FadeOutAnim);
	BindToAnimationFinished(FadeOutAnim, FadeFinishedDelegate);
}

void UDeathScreenUI::Restart()
{
	if (bAlreadyClickedButton) return;
	PlaySound(ButtonClickSound);
	bAlreadyClickedButton = true;
	LevelNameToLoad = FName("Level_1");
	FadeFinishedDelegate.BindDynamic(this, &UDeathScreenUI::FadeOutToNextLevel);
	PlayAnimation(FadeOutAnim);
	BindToAnimationFinished(FadeOutAnim, FadeFinishedDelegate);
}

void UDeathScreenUI::FadeOutToNextLevel()
{
	GetGameInstance()->GetSubsystem<UDelegateManager>()->LoadLevel.ExecuteIfBound(FLoadLevelData(true, false, true, LevelNameToLoad));
}

void UDeathScreenUI::PlayHoverSound() { PlaySound(ButtonHoverSound); }
