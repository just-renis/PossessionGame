#include "WidgetManager.h"
#include "SaveSlotManager.h"
#include "PossessionGame/UI/MainWidgets/ExplanationUI.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/SaveSlot.h"

void UWidgetManager::Init(const TMap<EWidget, TSubclassOf<UUserWidget>>& InWidgetClasses)
{
	WidgetClasses = InWidgetClasses;
	OnShowExplanation.BindUObject(this, &UWidgetManager::ShowExplanation);
}

void UWidgetManager::DestroyWidget(const EWidget InWidget)
{
	if (Widgets.Contains(InWidget))
	{
		if (TObjectPtr<UUserWidget> Widget = Widgets[InWidget])
		{
			Widget->RemoveFromParent();
			Widget = nullptr;
			Widgets.Remove(InWidget);
		}
	}
}

int32 UWidgetManager::GetWidgetZOrder(const EWidget InWidgetName)
{
	switch (InWidgetName)
	{
		case EWidget::WaveManagerHUD: case EWidget::PlayerHUD: case EWidget::MainMenuUI: case EWidget::TutorialUI: case EWidget::HijackUI: case EWidget::PlayerGhostNonTutorialUI: return 0;
		case EWidget::UpgradeSelectionUI: case EWidget::VariationsUI: case EWidget::SecretRoomUI: case EWidget::AbilitiesMenuUI: case EWidget::PlayerLevelSystemUI: return 1;
		case EWidget::ExplanationUI: return 2;
		case EWidget::SettingsUI: case EWidget::PauseMenuUI: case EWidget::SaveSlotMenuUI: return 3;
		case EWidget::LoadingScreenUI: case EWidget::GameLaunchUI: case EWidget::DemoEndUI: return 4;
		default: return 5;
	}
}

void UWidgetManager::ShowExplanation(const EExplanation InExplanationName, const TObjectPtr<UGameInstance>& InGameInstance)
{
	InGameInstance->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->AddShownExplanation(InExplanationName);
	GetWidget<UExplanationUI>(EWidget::ExplanationUI)->OpenUI(InExplanationName);
}
