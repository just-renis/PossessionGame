#include "SaveSlotMenuUI.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/UI/SecondaryWidgets/ConfirmationPopupUI.h"
#include "PossessionGame/UI/SecondaryWidgets/MainMenuButtonUI.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/SaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void USaveSlotMenuUI::OpenUI() { Toggle(true); }

void USaveSlotMenuUI::CloseUI() { Toggle(false); }

void USaveSlotMenuUI::NativeConstruct()
{
	Super::NativeConstruct();
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnSaveSlotClicked.BindUObject(this, &USaveSlotMenuUI::OpenConfirmationPopup);
}

void USaveSlotMenuUI::NativeDestruct()
{
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnSaveSlotClicked.Unbind();
	Super::NativeDestruct();
}

void USaveSlotMenuUI::Toggle(const bool bState)
{
	SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	GetWorld()->GetSubsystem<UWidgetManager>()->SetCurrentStateOfUI(bState ? EWidget::SaveSlotMenuUI : EWidget::None);
}

void USaveSlotMenuUI::OpenConfirmationPopup(const FString& InSlotName, const FString& InRunSlotName)
{
	if (!ConfirmationPopup) return;
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
	const TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->bShowMouseCursor = false;
	PlayerController->SetInputMode(FInputModeGameOnly());
}

void USaveSlotMenuUI::OpenCurrentSaveLevel()
{
	ResetToDefaultPlayerInputMode();
	if (!GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetIsTutorialFinished())
	{
		UGameplayStatics::OpenLevel(GetWorld(), "TutorialLevel");
		return;
	}
	switch (GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetCurrentChapter())
	{
		case 1: UGameplayStatics::OpenLevel(GetWorld(), "Level_1");
		default: ;
	}
}

void USaveSlotMenuUI::OverwriteSaveAndOpenLevel()
{
	ResetToDefaultPlayerInputMode();
	GetGameInstance()->GetSubsystem<USaveSlotManager>()->ResetSaveSlot();
	GetGameInstance()->GetSubsystem<USaveSlotManager>()->ResetRunSaveSlot();
	UGameplayStatics::OpenLevel(GetWorld(), "TutorialLevel");
}

void USaveSlotMenuUI::LoadSaveAndOpenLevel()
{
	ResetToDefaultPlayerInputMode();
	GetGameInstance()->GetSubsystem<USaveSlotManager>()->LoadSaveSlot(SaveSlotName);
	GetGameInstance()->GetSubsystem<USaveSlotManager>()->LoadRunSaveSlot(RunSaveSlotName);
	UGameplayStatics::OpenLevel(GetWorld(), "TutorialLevel");
}

void USaveSlotMenuUI::GoBack() { ConfirmationPopup->PlayPopupFadeInAnim(false); }
