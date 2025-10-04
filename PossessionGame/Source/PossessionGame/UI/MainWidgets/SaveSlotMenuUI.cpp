#include "SaveSlotMenuUI.h"
#include "MainMenuUI.h"
#include "Components/Button.h"
#include "PossessionGame/UI/SecondaryWidgets/CloseButtonUI.h"
#include "PossessionGame/UI/SecondaryWidgets/ConfirmationPopupUI.h"
#include "PossessionGame/UI/SecondaryWidgets/MainMenuButtonUI.h"
#include "PossessionGame/UI/SecondaryWidgets/ModeSelectUI.h"
#include "PossessionGame/Utilities/Data Utilities/FLoadLevelData.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/SaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void USaveSlotMenuUI::OpenUI() { Toggle(true); }

void USaveSlotMenuUI::PlayBlackFadeOutAnim() { PlayAnimation(BlackFadeOutAnim); }

void USaveSlotMenuUI::CloseUI()
{
	if (bIsModeSelectionOpen)
	{
		TriggerSelectionMenuFadeOut();
		return;
	}
	if (bIsConfirmationPanelOpen)
	{
		ToggleSaveSlots(true);
		GoBack();
		return;
	}
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UMainMenuUI>(EWidget::MainMenuUI)->TriggerMainMenuFadeInAnim();
	Toggle(false);
}

void USaveSlotMenuUI::NativeConstruct()
{
	Super::NativeConstruct();
	CloseButton->Get()->OnClicked.AddDynamic(this, &USaveSlotMenuUI::CloseUI);
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnSaveSlotClicked.BindUObject(this, &USaveSlotMenuUI::OpenConfirmationPopup);
	ModeSelection->SetParentWidget(this);
	ModeSelection->GetBackButton()->OnClicked.AddDynamic(this, &USaveSlotMenuUI::TriggerSelectionMenuFadeOut);
	
}

void USaveSlotMenuUI::NativeDestruct()
{
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnSaveSlotClicked.Unbind();
	Super::NativeDestruct();
}

void USaveSlotMenuUI::Toggle(const bool bState)
{
	if (!bState)
	{
		FadeInAnimFinishedDelegate.BindDynamic(this, &USaveSlotMenuUI::FadeOutAnimFinished);
		BindToAnimationFinished(FadeInAnim, FadeInAnimFinishedDelegate);
	}
	else
	{
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		ToggleSaveSlots(true);
	}
	PlayAnimation(FadeInAnim, 0, 1, bState ? EUMGSequencePlayMode::Forward : EUMGSequencePlayMode::Reverse);
	GetWorld()->GetSubsystem<UWidgetManager>()->SetCurrentStateOfUI(bState ? EWidget::SaveSlotMenuUI : EWidget::MainMenuUI);
}

void USaveSlotMenuUI::OpenConfirmationPopup(const FString& InSlotName, const FString& InRunSlotName)
{
	if (!ConfirmationPopup) return;
	ToggleSaveSlots(false);
	bIsConfirmationPanelOpen = true;
	SaveSlotName = InSlotName;
	RunSaveSlotName = InRunSlotName;
	ConfirmationPopup->PlayPopupFadeInAnim(true);
	ConfirmationPopup->GetAcceptButton()->Get()->OnClicked.Clear();
	ConfirmationPopup->GetDeclineButton()->Get()->OnClicked.Clear();
	if (GetGameInstance()->GetSubsystem<USaveSlotManager>()->DoesSaveSlotExists(InSlotName))
	{
		GetGameInstance()->GetSubsystem<USaveSlotManager>()->LoadSaveSlot(InSlotName);
		GetGameInstance()->GetSubsystem<USaveSlotManager>()->LoadRunSaveSlot(InRunSlotName);
		ConfirmationPopup->SetTitleText(NSLOCTEXT("ST_Localization", "ContinueOrOverwrite", "Continue or Overwrite?"));
		ConfirmationPopup->GetAcceptButton()->SetText(NSLOCTEXT("ST_Localization", "Continue", "Continue"));
		ConfirmationPopup->GetDeclineButton()->SetText(NSLOCTEXT("ST_Localization", "Overwrite", "Overwrite"));
		ConfirmationPopup->GetAcceptButton()->Get()->OnClicked.AddDynamic(this, &USaveSlotMenuUI::OpenCurrentSaveLevel);
		ConfirmationPopup->GetDeclineButton()->Get()->OnClicked.AddDynamic(this, &USaveSlotMenuUI::OverwriteSaveAndOpenLevel);
	}
	else
	{
		ConfirmationPopup->SetTitleText(NSLOCTEXT("ST_Localization", "StartNewGame", "Start new game?"));
		ConfirmationPopup->GetAcceptButton()->SetText(NSLOCTEXT("ST_Localization", "Yes", "Yes"));
		ConfirmationPopup->GetDeclineButton()->SetText(NSLOCTEXT("ST_Localization", "No", "No"));
		ConfirmationPopup->GetAcceptButton()->Get()->OnClicked.AddDynamic(this, &USaveSlotMenuUI::LoadSaveAndOpenLevel);
		ConfirmationPopup->GetDeclineButton()->Get()->OnClicked.AddDynamic(this, &USaveSlotMenuUI::GoBack);
	}
}

void USaveSlotMenuUI::ToggleSaveSlots(const bool bState) const
{
	SaveSlot1->SetIsEnabled(bState);
	SaveSlot2->SetIsEnabled(bState);
	SaveSlot3->SetIsEnabled(bState);
	SaveSlot4->SetIsEnabled(bState);
}

void USaveSlotMenuUI::FadeOutAnimFinished()
{
	UnbindFromAnimationFinished(FadeInAnim, FadeInAnimFinishedDelegate);
	SetVisibility(ESlateVisibility::Collapsed);
}

void USaveSlotMenuUI::TriggerSelectionMenuFadeOut()
{
	if (!ModeSelection) return;
	ModeSelection->PlayFadeInAnim(false);
	bIsModeSelectionOpen = false;
	OpenUI();
}

void USaveSlotMenuUI::OpenCurrentSaveLevel()
{
	bIsModeSelectionOpen = true;
	PlayAnimation(FadeOutAnim);
	if (!GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetIsTutorialFinished())
	{
		ModeSelection->PlayFadeInAnim(true, FLoadLevelData(false, true, false, FName("TutorialLevel")));
		GoBack();
		return;
	}
	switch (GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetCurrentChapter())
	{
		case 1:
			ModeSelection->PlayFadeInAnim(true, FLoadLevelData(false, false, false, FName("Level_1")));
			GoBack();
		default: ;
	}
}

void USaveSlotMenuUI::OverwriteSaveAndOpenLevel()
{
	PlayAnimation(FadeOutAnim);
	ModeSelection->PlayFadeInAnim(true, FLoadLevelData(false, true, true, FName("TutorialLevel")));
	GoBack();
}

void USaveSlotMenuUI::LoadSaveAndOpenLevel()
{
	GetGameInstance()->GetSubsystem<USaveSlotManager>()->LoadSaveSlot(SaveSlotName);
	GetGameInstance()->GetSubsystem<USaveSlotManager>()->LoadRunSaveSlot(RunSaveSlotName);
	PlayAnimation(FadeOutAnim);
	ModeSelection->PlayFadeInAnim(true, FLoadLevelData(false, false, false, FName("TutorialLevel")));
	GoBack();
}

void USaveSlotMenuUI::GoBack()
{
	bIsConfirmationPanelOpen = false;
	ConfirmationPopup->PlayPopupFadeInAnim(false);
}
