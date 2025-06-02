#include "UpgradeSelectionUI.h"

#include "PlayerHUD.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Data Assets/Upgrades/UpgradeData.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EWidget.h"
#include "PossessionGame/UI/SecondaryWidgets/MainMenuButtonUI.h"
#include "PossessionGame/UI/SecondaryWidgets/UpgradeCardUI.h"
#include "PossessionGame/UI/SecondaryWidgets/UpgradeDetailsPanelUI.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/UpgradeManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void UUpgradeSelectionUI::OpenUI()
{
	GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::UpgradePanelAppear);
	(void)DelegateManager->OnUpgradeSelectionOpened.ExecuteIfBound();
	Toggle(true);
	PlayAnimation(UpgradeMenuOpenAnim);
	UpgradesData = GetWorld()->GetSubsystem<UUpgradeManager>()->GetRandomUpgrades();
	if (UpgradesData.Num() > 0)
	{
		if (PlayerCardOption1)
		{
			PlayerCardOption1->SetupCard(true, PlayerCardBackgroundTexture, UpgradesData[0], UpgradesData[0]->GetCharacter() == ECharacterType::Ranger ? RangerIconTexture : TricksterIconTexture, PlayerIconTexture);
			PlayerCardOption1->SetUpgradeText(GetUpgradeTextFromUpgrade(UpgradesData[0]->GetUpgradeName()));
		}
		if (EnemyCardOption1) EnemyCardOption1->SetupCard(false, EnemyCardBackgroundTexture, UpgradesData[0], EnemyIconTexture);
	}
	if (UpgradesData.Num() > 1)
	{
		if (PlayerCardOption2)
		{
			PlayerCardOption2->SetupCard(true, PlayerCardBackgroundTexture, UpgradesData[1], UpgradesData[1]->GetCharacter() == ECharacterType::Ranger ? RangerIconTexture : TricksterIconTexture, PlayerIconTexture);
			PlayerCardOption2->SetUpgradeText(GetUpgradeTextFromUpgrade(UpgradesData[1]->GetUpgradeName()));
		}
		if (EnemyCardOption2) EnemyCardOption2->SetupCard(false, EnemyCardBackgroundTexture, UpgradesData[1], EnemyIconTexture);
	}
}

void UUpgradeSelectionUI::CloseUI()
{
	if (PlayerCardOption1) PlayerCardOption1->ToggleCardVisibility(false);
	if (EnemyCardOption1) EnemyCardOption1->ToggleCardVisibility(false);
	if (PlayerCardOption2) PlayerCardOption2->ToggleCardVisibility(false);
	if (EnemyCardOption2) EnemyCardOption2->ToggleCardVisibility(false);
	if (UpgradeChoicesPanel) UpgradeChoicesPanel->SetVisibility(ESlateVisibility::Collapsed);
	Toggle(false);
}

void UUpgradeSelectionUI::TriggerUpgradeMenuCloseAnimation()
{
	if (UpgradeChoicesPanel) UpgradeChoicesPanel->SetIsEnabled(false);
	UpgradeChoice1->PlayDisappearAnim();
	UpgradeChoice2->PlayDisappearAnim();
	UpgradeChoice3->PlayDisappearAnim();
	PlayAnimation(UpgradeMenuCloseAnim);
	PlayAnimation(bFirstOptionSelected ? UpgradeSelectFirstAnim : UpgradeSelectSecondAnim, 0, 1, EUMGSequencePlayMode::Reverse);
	BindToAnimationFinished(UpgradeMenuCloseAnim, UpgradeSelectionFadeOutDelegate);
}

void UUpgradeSelectionUI::GoBack()
{
	if (bChoicesOpen)
	{
		PlayAnimation(bFirstOptionSelected ? UpgradeSelectFirstAnim : UpgradeSelectSecondAnim, 0, 1, EUMGSequencePlayMode::Reverse);
		bChoicesOpen = false;
		PlayerCardOption1->GetUpgradeButton()->SetIsEnabled(true);
		EnemyCardOption1->GetUpgradeButton()->SetIsEnabled(true);
		PlayerCardOption2->GetUpgradeButton()->SetIsEnabled(true);
		EnemyCardOption2->GetUpgradeButton()->SetIsEnabled(true);
		if (BackButton) BackButton->SetVisibility(ESlateVisibility::Collapsed);
	}
}

FString UUpgradeSelectionUI::GetUpgradeTextFromUpgrade(const EUpgrade InUpgrade)
{
	switch (InUpgrade)
	{
		case EUpgrade::RangerAbility1: return " GRENADE LAUNCHER UPGRADE";
		case EUpgrade::RangerAbility2: return " SHIELD UPGRADE";
		case EUpgrade::TricksterAbility1: return " POISON UPGRADE";
		case EUpgrade::TricksterAbility2: return " EMP UPGRADE";
	}
	return "";
}

void UUpgradeSelectionUI::Toggle(const bool bState)
{
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UPlayerHUD>(EWidget::PlayerHUD)->PlayFadeOutAnim(bState);
	UGameplayStatics::SetGamePaused(GetWorld(), bState);
	SetIsEnabled(bState);
	SetVisibility(bState ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(bState ? ESound::PanelFadeIn : ESound::PanelFadeOut);
	const TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->bShowMouseCursor = bState;
	if (bState) PlayerController->SetInputMode(FInputModeGameAndUI());
	else PlayerController->SetInputMode(FInputModeGameOnly());
	if (bState && UpgradeChoicesPanel) UpgradeChoicesPanel->SetIsEnabled(true);
	GetWorld()->GetSubsystem<UWidgetManager>()->SetCurrentStateOfUI(bState ? EWidget::UpgradeSelectionUI : EWidget::None);
}

void UUpgradeSelectionUI::NativeConstruct()
{
	Super::NativeConstruct();
	DelegateManager = GetGameInstance()->GetSubsystem<UDelegateManager>();
	DelegateManager->OnUpgradeCardSelected.BindUObject(this, &UUpgradeSelectionUI::OpenUpgradeChoicesPanel);
	UpgradeSelectionFadeOutDelegate.BindDynamic(this, &UUpgradeSelectionUI::UpgradeSelectionPanelFadeOutFinished);
	if (PlayerCardOption1)
	{
		PlayerCardOption1->GetUpgradeButton()->OnHovered.AddDynamic(this, &UUpgradeSelectionUI::TriggerHoverAnimForFirstOption);
		PlayerCardOption1->GetUpgradeButton()->OnUnhovered.AddDynamic(this, &UUpgradeSelectionUI::TriggerUnHoverAnimForFirstOption);
		PlayerCardOption1->GetUpgradeButton()->OnClicked.AddDynamic(this, &UUpgradeSelectionUI::TriggerSelectAnimForFirstOption);
	}
	if (PlayerCardOption2)
	{
		PlayerCardOption2->GetUpgradeButton()->OnHovered.AddDynamic(this, &UUpgradeSelectionUI::TriggerHoverAnimForSecondOption);
		PlayerCardOption2->GetUpgradeButton()->OnUnhovered.AddDynamic(this, &UUpgradeSelectionUI::TriggerUnHoverAnimForSecondOption);
		PlayerCardOption2->GetUpgradeButton()->OnClicked.AddDynamic(this, &UUpgradeSelectionUI::TriggerSelectAnimForSecondOption);
	}
	if (EnemyCardOption1)
	{
		EnemyCardOption1->GetUpgradeButton()->OnHovered.AddDynamic(this, &UUpgradeSelectionUI::TriggerHoverAnimForFirstOption);
		EnemyCardOption1->GetUpgradeButton()->OnUnhovered.AddDynamic(this, &UUpgradeSelectionUI::TriggerUnHoverAnimForFirstOption);
		EnemyCardOption1->GetUpgradeButton()->OnClicked.AddDynamic(this, &UUpgradeSelectionUI::TriggerSelectAnimForFirstOption);
	}
	if (EnemyCardOption2)
	{
		EnemyCardOption2->GetUpgradeButton()->OnHovered.AddDynamic(this, &UUpgradeSelectionUI::TriggerHoverAnimForSecondOption);
		EnemyCardOption2->GetUpgradeButton()->OnUnhovered.AddDynamic(this, &UUpgradeSelectionUI::TriggerUnHoverAnimForSecondOption);
		EnemyCardOption2->GetUpgradeButton()->OnClicked.AddDynamic(this, &UUpgradeSelectionUI::TriggerSelectAnimForSecondOption);
	}
	if (BackButton)
	{
		BackButton->Get()->OnClicked.AddDynamic(this, &UUpgradeSelectionUI::GoBack);
		BackButton->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UUpgradeSelectionUI::OpenUpgradeChoicesPanel(const TObjectPtr<UUpgradeData>& InUpgradeData)
{
	if (!UpgradeChoicesPanel) return;
	bChoicesOpen = true;
	if (BackButton) BackButton->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	PlayerCardOption1->GetUpgradeButton()->SetIsEnabled(false);
	EnemyCardOption1->GetUpgradeButton()->SetIsEnabled(false);
	PlayerCardOption2->GetUpgradeButton()->SetIsEnabled(false);
	EnemyCardOption2->GetUpgradeButton()->SetIsEnabled(false);
	UpgradeChoice1->Setup(InUpgradeData);
	UpgradeChoice2->Setup(InUpgradeData);
	UpgradeChoice3->Setup(InUpgradeData);
	UpgradeChoicesPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UUpgradeSelectionUI::TriggerHoverAnimForFirstOption()
{
	GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::ButtonHover);
	PlayerCardOption1->StopUnHoverAnim();
	EnemyCardOption1->StopUnHoverAnim();
	PlayerCardOption1->PlayHoverAnim();
	EnemyCardOption1->PlayHoverAnim();
}

void UUpgradeSelectionUI::TriggerUnHoverAnimForFirstOption()
{
	PlayerCardOption1->StopHoverAnim();
	EnemyCardOption1->StopHoverAnim();
	PlayerCardOption1->PlayUnHoverAnim();
	EnemyCardOption1->PlayUnHoverAnim();
}

void UUpgradeSelectionUI::TriggerHoverAnimForSecondOption()
{
	GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::ButtonHover);
	PlayerCardOption2->StopUnHoverAnim();
	EnemyCardOption2->StopUnHoverAnim();
	PlayerCardOption2->PlayHoverAnim();
	EnemyCardOption2->PlayHoverAnim();
}

void UUpgradeSelectionUI::TriggerUnHoverAnimForSecondOption()
{
	PlayerCardOption2->StopHoverAnim();
	EnemyCardOption2->StopHoverAnim();
	PlayerCardOption2->PlayUnHoverAnim();
	EnemyCardOption2->PlayUnHoverAnim();
}

void UUpgradeSelectionUI::TriggerSelectAnimForFirstOption()
{
	bFirstOptionSelected = true;
	PlayerCardOption1->PlaySelectAnim();
	EnemyCardOption1->PlaySelectAnim();
	PlayerCardOption1->PlayBlinkAnim();
	EnemyCardOption1->PlayBlinkAnim();
	PlayAnimation(UpgradeSelectFirstAnim);
}

void UUpgradeSelectionUI::TriggerSelectAnimForSecondOption()
{
	bFirstOptionSelected = false;
	PlayerCardOption2->PlaySelectAnim();
	EnemyCardOption2->PlaySelectAnim();
	PlayerCardOption2->PlayBlinkAnim();
	EnemyCardOption2->PlayBlinkAnim();
	PlayAnimation(UpgradeSelectSecondAnim);
}

void UUpgradeSelectionUI::UpgradeSelectionPanelFadeOutFinished()
{
	UnbindAllFromAnimationFinished(UpgradeMenuCloseAnim);
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UUpgradeSelectionUI>(EWidget::UpgradeSelectionUI)->CloseUI();
	(void)DelegateManager->OnUpgradeSelectionFinished.ExecuteIfBound();
}
