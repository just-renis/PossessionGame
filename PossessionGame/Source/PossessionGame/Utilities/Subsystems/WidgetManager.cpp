#include "WidgetManager.h"

void UWidgetManager::Init(const TMap<EWidget, TSubclassOf<UUserWidget>>& InWidgetClasses) { WidgetClasses = InWidgetClasses; }

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
		case EWidget::WaveManagerHUD: return 0;
		case EWidget::PlayerHUD: return 0;
		case EWidget::MainMenuUI: return 0;
		case EWidget::SettingsUI: return 2;
		case EWidget::PauseMenuUI: return 2;
		case EWidget::UpgradeSelectionUI: return 1;
		case EWidget::TutorialUI: return 0;
		case EWidget::VariationsUI: return 1;
		case EWidget::LoadingScreenUI: return 3;
		case EWidget::GameLaunchUI: return 3;
		case EWidget::DemoEndUI: return 3;
		case EWidget::SecretRoomUI: return 1;
		case EWidget::AbilitiesMenuUI: return 1;
		default: return 4;
	}
}
