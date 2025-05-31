#include "UpgradeSelectionUI.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "PossessionGame/Data Assets/Upgrades/UpgradeData.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EWidget.h"
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
	PlayAnimation(UpgradeMenuOpen);
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
	SetIsEnabled(bState);
	SetVisibility(bState ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(bState ? ESound::PanelFadeIn : ESound::PanelFadeOut);
	const TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->bShowMouseCursor = bState;
	if (bState)
	{
		if (UpgradeChoicesPanel) UpgradeChoicesPanel->SetIsEnabled(true);
		PlayerController->SetInputMode(FInputModeUIOnly());
		PlayerController->FlushPressedKeys();
	}
	else PlayerController->SetInputMode(FInputModeGameOnly());
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
}

void UUpgradeSelectionUI::OpenUpgradeChoicesPanel(const TObjectPtr<UUpgradeData>& InUpgradeData) const
{
	if (!UpgradeChoicesPanel) return;
	PlayerCardOption1->SetVisibility(ESlateVisibility::Collapsed);
	PlayerCardOption2->SetVisibility(ESlateVisibility::Collapsed);
	EnemyCardOption1->SetVisibility(ESlateVisibility::Collapsed);
	EnemyCardOption2->SetVisibility(ESlateVisibility::Collapsed);
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
	PlayerCardOption1->PlaySelectAnim();
	EnemyCardOption1->PlaySelectAnim();
	PlayerCardOption1->PlayBlinkAnim();
	EnemyCardOption1->PlayBlinkAnim();
}

void UUpgradeSelectionUI::TriggerSelectAnimForSecondOption()
{
	PlayerCardOption2->PlaySelectAnim();
	EnemyCardOption2->PlaySelectAnim();
	PlayerCardOption2->PlayBlinkAnim();
	EnemyCardOption2->PlayBlinkAnim();
}

void UUpgradeSelectionUI::UpgradeSelectionPanelFadeOutFinished()
{
	UnbindAllFromAnimationFinished(UpgradeMenuClose);
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UUpgradeSelectionUI>(EWidget::UpgradeSelectionUI)->CloseUI();
	(void)DelegateManager->OnUpgradeSelectionFinished.ExecuteIfBound();
}

void UUpgradeSelectionUI::TriggerUpgradeMenuCloseAnimation()
{
	if (UpgradeChoicesPanel) UpgradeChoicesPanel->SetIsEnabled(false);
	PlayAnimation(UpgradeMenuClose);
	BindToAnimationFinished(UpgradeMenuClose, UpgradeSelectionFadeOutDelegate);
}
