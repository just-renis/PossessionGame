#include "UpgradeSelectionUI.h"
#include "PlayerHUD.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Data Assets/Upgrades/UpgradeData.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EExplanation.h"
#include "PossessionGame/Enums/EWidget.h"
#include "PossessionGame/UI/SecondaryWidgets/MainMenuButtonUI.h"
#include "PossessionGame/UI/SecondaryWidgets/UpgradeCardUI.h"
#include "PossessionGame/UI/SecondaryWidgets/UpgradeDetailsPanelUI.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/SaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"
#include "PossessionGame/Utilities/Subsystems/UpgradeManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void UUpgradeSelectionUI::OpenUI()
{
	UpgradesData = GetWorld()->GetSubsystem<UUpgradeManager>()->GetRandomUpgrades();
	if (UpgradesData.IsEmpty())
	{
		CloseUI();
		(void)DelegateManager->OnUpgradeSelectionFinished.ExecuteIfBound();
		return;
	}
	PlayAnimation(UpgradeMenuOpenAnim);
	(void)DelegateManager->OnUpgradeSelectionOpened.ExecuteIfBound();
	Toggle(true);
	for (int32 i = 0; i < UpgradesData.Num(); i++)
	{
		CardOptions[i]->SetupCard(PlayerCardBackgroundTexture, UpgradesData[i], GetCharacterUpgradeIcon(UpgradesData[i]->GetCharacter()));
		CardOptions[i]->SetUpgradeText(GetUpgradeTextFromUpgrade(UpgradesData[i]->GetUpgradeName()));
	}
	if (!GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetIsExplanationShown(EExplanation::UpgradeUI))
	{
		(void)GetWorld()->GetSubsystem<UWidgetManager>()->GetOnShowExplanationDelegate().ExecuteIfBound(EExplanation::UpgradeUI, GetGameInstance());
	}
}

void UUpgradeSelectionUI::CloseUI()
{
	for (int32 i = 0; i < UpgradesData.Num(); i++) CardOptions[i]->ToggleCardVisibility(false);
	UpgradeChoicesPanel->SetVisibility(ESlateVisibility::Collapsed);
	Toggle(false);
}

void UUpgradeSelectionUI::TriggerUpgradeMenuCloseAnimation()
{
	UpgradeChoice1->PlayDisappearAnim();
	UpgradeChoice2->PlayDisappearAnim();
	UpgradeChoice3->PlayDisappearAnim();
	PlayAnimation(UpgradeMenuCloseAnim);
	PlayAnimation(bFirstOptionSelected ? UpgradeSelectFirstAnim : UpgradeSelectSecondAnim, 0, 1, EUMGSequencePlayMode::Reverse);
	BindToAnimationFinished(UpgradeMenuCloseAnim, UpgradeSelectionFadeOutDelegate);
}

void UUpgradeSelectionUI::GoBack()
{
	if (!bChoicesOpen) return;
	BindToAnimationFinished(bFirstOptionSelected ? UpgradeSelectFirstAnim : UpgradeSelectSecondAnim, GoBackAnimFinishedDelegate);
	PlayAnimation(bFirstOptionSelected ? UpgradeSelectFirstAnim : UpgradeSelectSecondAnim, 0, 1, EUMGSequencePlayMode::Reverse);
	bChoicesOpen = false;
	UpgradeChoice1->SetButtonVisibility(false);
	UpgradeChoice2->SetButtonVisibility(false);
	UpgradeChoice3->SetButtonVisibility(false);
	BackButton->SetButtonVisibility(false);
}

FText UUpgradeSelectionUI::GetUpgradeTextFromUpgrade(const EUpgrade InUpgrade)
{
	switch (InUpgrade)
	{
		case EUpgrade::RangerAbility1: return NSLOCTEXT("ST_Localization", "GrenadeLauncherUpgrade", " GRENADE LAUNCHER");
		case EUpgrade::RangerAbility2: return NSLOCTEXT("ST_Localization", "ShieldUpgrade", " SHIELD");
		case EUpgrade::TricksterAbility1: return NSLOCTEXT("ST_Localization", "PoisonUpgrade", " POISON");
		case EUpgrade::TricksterAbility2: return NSLOCTEXT("ST_Localization", "EmpUpgrade", " EMP");
		case EUpgrade::GuardianAbility1: return NSLOCTEXT("ST_Localization", "ShieldUpgrade", " SHIELD");
		case EUpgrade::GuardianAbility2: return NSLOCTEXT("ST_Localization", "TimeStopUpgrade", " TIME STOP");
	}
	return FText::GetEmpty();
}

TObjectPtr<UTexture2D>& UUpgradeSelectionUI::GetCharacterUpgradeIcon(const ECharacterType InCharacter)
{
	switch (InCharacter)
	{
		case ECharacterType::Guardian: return GuardianIconTexture;
		case ECharacterType::Ranger: return RangerIconTexture;
		case ECharacterType::Trickster: return TricksterIconTexture;
		default: ;
	}
	return GuardianIconTexture;
}

void UUpgradeSelectionUI::Toggle(const bool bState)
{
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UPlayerHUD>(EWidget::PlayerHUD)->PlayFadeOutAnim(bState);
	UGameplayStatics::SetGamePaused(GetWorld(), bState);
	SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(bState ? ESound::UpgradePanelFadeIn : ESound::UpgradePanelFadeOut);
	const TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->bShowMouseCursor = bState;
	if (bState) PlayerController->SetInputMode(FInputModeGameAndUI());
	else PlayerController->SetInputMode(FInputModeGameOnly());
	GetWorld()->GetSubsystem<UWidgetManager>()->SetCurrentStateOfUI(bState ? EWidget::UpgradeSelectionUI : EWidget::None);
}

void UUpgradeSelectionUI::NativeConstruct()
{
	Super::NativeConstruct();
	DelegateManager = GetGameInstance()->GetSubsystem<UDelegateManager>();
	DelegateManager->OnUpgradeCardSelected.BindUObject(this, &UUpgradeSelectionUI::OpenUpgradeChoicesPanel);
	UpgradeSelectionFadeOutDelegate.BindDynamic(this, &UUpgradeSelectionUI::UpgradeSelectionPanelFadeOutFinished);
	GoBackAnimFinishedDelegate.BindDynamic(this, &UUpgradeSelectionUI::GoBackAnimFinished);
	CardOptions = { CardOption1, CardOption2, CardOption3, CardOption4 };
	CardOption1->GetUpgradeButton()->OnHovered.AddDynamic(this, &UUpgradeSelectionUI::TriggerHoverAnimForFirstOption);
	CardOption1->GetUpgradeButton()->OnUnhovered.AddDynamic(this, &UUpgradeSelectionUI::TriggerUnHoverAnimForFirstOption);
	CardOption1->GetUpgradeButton()->OnClicked.AddDynamic(this, &UUpgradeSelectionUI::TriggerSelectAnimForFirstOption);
	CardOption2->GetUpgradeButton()->OnHovered.AddDynamic(this, &UUpgradeSelectionUI::TriggerHoverAnimForSecondOption);
	CardOption2->GetUpgradeButton()->OnUnhovered.AddDynamic(this, &UUpgradeSelectionUI::TriggerUnHoverAnimForSecondOption);
	CardOption2->GetUpgradeButton()->OnClicked.AddDynamic(this, &UUpgradeSelectionUI::TriggerSelectAnimForSecondOption);
	CardOption3->GetUpgradeButton()->OnHovered.AddDynamic(this, &UUpgradeSelectionUI::TriggerHoverAnimForFirstOption);
	CardOption3->GetUpgradeButton()->OnUnhovered.AddDynamic(this, &UUpgradeSelectionUI::TriggerUnHoverAnimForFirstOption);
	CardOption3->GetUpgradeButton()->OnClicked.AddDynamic(this, &UUpgradeSelectionUI::TriggerSelectAnimForFirstOption);
	CardOption4->GetUpgradeButton()->OnHovered.AddDynamic(this, &UUpgradeSelectionUI::TriggerHoverAnimForSecondOption);
	CardOption4->GetUpgradeButton()->OnUnhovered.AddDynamic(this, &UUpgradeSelectionUI::TriggerUnHoverAnimForSecondOption);
	CardOption4->GetUpgradeButton()->OnClicked.AddDynamic(this, &UUpgradeSelectionUI::TriggerSelectAnimForSecondOption);
	BackButton->Get()->OnClicked.AddDynamic(this, &UUpgradeSelectionUI::GoBack);
	BackButton->SetButtonVisibility(false);
}

void UUpgradeSelectionUI::OpenUpgradeChoicesPanel(const TObjectPtr<UUpgradeData>& InUpgradeData)
{
	if (!UpgradeChoicesPanel) return;
	if (!GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetIsExplanationShown(EExplanation::UpgradeChoiceUI))
	{
		(void)GetWorld()->GetSubsystem<UWidgetManager>()->GetOnShowExplanationDelegate().ExecuteIfBound(EExplanation::UpgradeChoiceUI, GetGameInstance());
	}
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::UpgradeChoicesFadeIn);
	bChoicesOpen = true;
	BackButton->SetButtonVisibility(true);
	for (int32 i = 0; i < UpgradesData.Num(); i++) CardOptions[i]->GetUpgradeButton()->SetIsEnabled(false);
	UpgradeChoice1->Setup(InUpgradeData);
	UpgradeChoice2->Setup(InUpgradeData);
	UpgradeChoice3->Setup(InUpgradeData);
	UpgradeChoicesPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UUpgradeSelectionUI::TriggerHoverAnimForFirstOption()
{
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::ButtonHover);
	CardOption1->StopUnHoverAnim();
	CardOption1->PlayHoverAnim();
}

void UUpgradeSelectionUI::TriggerUnHoverAnimForFirstOption()
{
	CardOption1->StopHoverAnim();
	CardOption1->PlayUnHoverAnim();
}

void UUpgradeSelectionUI::TriggerHoverAnimForSecondOption()
{
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::ButtonHover);
	CardOption2->StopUnHoverAnim();
	CardOption2->PlayHoverAnim();
}

void UUpgradeSelectionUI::TriggerUnHoverAnimForSecondOption()
{
	CardOption2->StopHoverAnim();
	CardOption2->PlayUnHoverAnim();
}

void UUpgradeSelectionUI::TriggerSelectAnimForFirstOption()
{
	bFirstOptionSelected = true;
	CardOption1->PlaySelectAnim();
	CardOption1->PlayBlinkAnim();
	PlayAnimation(UpgradeSelectFirstAnim);
}

void UUpgradeSelectionUI::TriggerSelectAnimForSecondOption()
{
	bFirstOptionSelected = false;
	CardOption2->PlaySelectAnim();
	CardOption2->PlayBlinkAnim();
	PlayAnimation(UpgradeSelectSecondAnim);
}

void UUpgradeSelectionUI::UpgradeSelectionPanelFadeOutFinished()
{
	UnbindAllFromAnimationFinished(UpgradeMenuCloseAnim);
	CloseUI();
	(void)DelegateManager->OnUpgradeSelectionFinished.ExecuteIfBound();
}

void UUpgradeSelectionUI::GoBackAnimFinished()
{
	UnbindFromAnimationFinished(bFirstOptionSelected ? UpgradeSelectFirstAnim : UpgradeSelectSecondAnim, GoBackAnimFinishedDelegate);
	for (int32 i = 0; i < UpgradesData.Num(); i++) CardOptions[i]->GetUpgradeButton()->SetIsEnabled(true);
}
