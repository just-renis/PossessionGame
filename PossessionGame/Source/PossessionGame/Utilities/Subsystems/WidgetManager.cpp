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
		case EWidget::WaveManagerHUD: return 0;
		case EWidget::PlayerHUD: return 0;
		case EWidget::MainMenuUI: return 0;
		case EWidget::SettingsUI: return 3;
		case EWidget::PauseMenuUI: return 3;
		case EWidget::UpgradeSelectionUI: return 1;
		case EWidget::TutorialUI: return 0;
		case EWidget::VariationsUI: return 1;
		case EWidget::LoadingScreenUI: return 4;
		case EWidget::GameLaunchUI: return 4;
		case EWidget::DemoEndUI: return 4;
		case EWidget::SecretRoomUI: return 1;
		case EWidget::AbilitiesMenuUI: return 1;
		case EWidget::ExplanationUI: return 2;
		case EWidget::SaveSlotMenuUI: return 3;
		case EWidget::HijackUI: return 0;
		default: return 5;
	}
}

void UWidgetManager::ShowExplanation(const EExplanation InExplanationName, const TObjectPtr<UGameInstance>& InGameInstance)
{
	InGameInstance->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->AddShownExplanation(InExplanationName);
	GetWidget<UExplanationUI>(EWidget::ExplanationUI)->OpenUI(InExplanationName);
}
