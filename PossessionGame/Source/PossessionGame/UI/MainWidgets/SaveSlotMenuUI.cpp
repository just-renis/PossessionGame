#include "SaveSlotMenuUI.h"
#include "MainMenuUI.h"
#include "Components/Button.h"
#include "PossessionGame/UI/SecondaryWidgets/CloseButtonUI.h"
#include "PossessionGame/UI/SecondaryWidgets/ConfirmationPopupUI.h"
#include "PossessionGame/UI/SecondaryWidgets/MainMenuButtonUI.h"
#include "PossessionGame/Utilities/Data Utilities/FLoadLevelData.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/SaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void USaveSlotMenuUI::OpenUI(const bool bStoryMode)
{
	bEnterStoryMode = bStoryMode;
	Toggle(true);
}

void USaveSlotMenuUI::CloseUI()
{
	if (bIsConfirmationPanelOpen)
	{
		bIsConfirmationPanelOpen = false;
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
	else SetVisibility(ESlateVisibility::SelfHitTestInvisible);
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

void USaveSlotMenuUI::ResetToDefaultPlayerInputMode() const
{
	if (const TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController())
	{
		PlayerController->SetInputMode(FInputModeGameOnly());
		PlayerController->DisableInput(PlayerController);
		PlayerController->bShowMouseCursor = false;
	}
}

void USaveSlotMenuUI::ToggleSaveSlots(const bool bState) const
{
	SaveSlot1->SetIsEnabled(bState);
	SaveSlot2->SetIsEnabled(bState);
	SaveSlot3->SetIsEnabled(bState);
	SaveSlot4->SetIsEnabled(bState);
}

void USaveSlotMenuUI::LoadLevel() const { (void)GetGameInstance()->GetSubsystem<UDelegateManager>()->LoadLevel.ExecuteIfBound(PendingLoadLevelData); }

void USaveSlotMenuUI::FadeOutAnimFinished()
{
	UnbindFromAnimationFinished(FadeInAnim, FadeInAnimFinishedDelegate);
	SetVisibility(ESlateVisibility::Collapsed);
}

void USaveSlotMenuUI::OpenCurrentSaveLevel()
{
	ResetToDefaultPlayerInputMode();
	if (!GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetIsTutorialFinished())
	{
		PlayAnimation(FadeOutAnim);
		PendingLoadLevelData = FLoadLevelData(false, true, false, FName("TutorialLevel"));
		GetWorld()->GetTimerManager().SetTimer(LoadLevelHandle, this, &USaveSlotMenuUI::LoadLevel, 1.5f);
		return;
	}
	switch (GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetCurrentChapter())
	{
		case 1: 
			PlayAnimation(FadeOutAnim);
			PendingLoadLevelData = FLoadLevelData(false, false, false, bEnterStoryMode ? FName("Level_1") : FName("L_Arena"));
			GetWorld()->GetTimerManager().SetTimer(LoadLevelHandle, this, &USaveSlotMenuUI::LoadLevel, 1.5f);
		default: ;
	}
}

void USaveSlotMenuUI::OverwriteSaveAndOpenLevel()
{
	ResetToDefaultPlayerInputMode();
	PlayAnimation(FadeOutAnim);
	PendingLoadLevelData = FLoadLevelData(false, true, true, FName("TutorialLevel"));
	GetWorld()->GetTimerManager().SetTimer(LoadLevelHandle, this, &USaveSlotMenuUI::LoadLevel, 1.5f);
}

void USaveSlotMenuUI::LoadSaveAndOpenLevel()
{
	ResetToDefaultPlayerInputMode();
	GetGameInstance()->GetSubsystem<USaveSlotManager>()->LoadSaveSlot(SaveSlotName);
	GetGameInstance()->GetSubsystem<USaveSlotManager>()->LoadRunSaveSlot(RunSaveSlotName);
	PlayAnimation(FadeOutAnim);
	PendingLoadLevelData = FLoadLevelData(false, false, false, FName("TutorialLevel"));
	GetWorld()->GetTimerManager().SetTimer(LoadLevelHandle, this, &USaveSlotMenuUI::LoadLevel, 1.5f);
}

void USaveSlotMenuUI::GoBack() { ConfirmationPopup->PlayPopupFadeInAnim(false); }
