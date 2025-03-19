#include "UpgradeSelectionUI.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PossessionGame/Data Assets/Upgrades/UpgradeData.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EWidget.h"
#include "PossessionGame/UI/SecondaryWidgets/UpgradeCardUI.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/UpgradeManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void UUpgradeSelectionUI::OpenUI()
{
	(void)DelegateManager->OnUpgradeSelectionOpened.ExecuteIfBound();
	Toggle(true);
	if (UpgradeMenuOpen) PlayAnimation(UpgradeMenuOpen);
	const TArray<TObjectPtr<UUpgradeData>> Data = GetWorld()->GetSubsystem<UUpgradeManager>()->GetRandomUpgrades();
	if (Data.Num() > 0)
	{
		if (PlayerCardOption1) PlayerCardOption1->SetupCard(true, PlayerCardBackgroundTexture, Data[0], Data[0]->GetCharacter() == ECharacterType::Ranger ? RangerIconTexture : TricksterIconTexture, PlayerIconTexture);
		if (EnemyCardOption1) EnemyCardOption1->SetupCard(false, EnemyCardBackgroundTexture, Data[0], EnemyIconTexture);
	}
	if (Data.Num() > 1)
	{
		if (PlayerCardOption2) PlayerCardOption2->SetupCard(true, PlayerCardBackgroundTexture, Data[1], Data[1]->GetCharacter() == ECharacterType::Ranger ? RangerIconTexture : TricksterIconTexture, PlayerIconTexture);
		if (EnemyCardOption2) EnemyCardOption2->SetupCard(false, EnemyCardBackgroundTexture, Data[1], EnemyIconTexture);
	}
}

void UUpgradeSelectionUI::CloseUI()
{
	if (PlayerCardOption1) PlayerCardOption1->ToggleCardVisibility(false);
	if (EnemyCardOption1) EnemyCardOption1->ToggleCardVisibility(false);
	if (PlayerCardOption2) PlayerCardOption2->ToggleCardVisibility(false);
	if (EnemyCardOption2) EnemyCardOption2->ToggleCardVisibility(false);
	Toggle(false);
}

void UUpgradeSelectionUI::Toggle(const bool bState)
{
	SetVisibility(bState ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	const TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->bShowMouseCursor = bState;
	PlayerController->bShowMouseCursor = bState;
	if (bState)
	{
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
	DelegateManager->OnUpgradeSelected.BindUObject(this, &UUpgradeSelectionUI::TriggerUpgradeMenuCloseAnimation);
	if (PlayerCardOption1)
	{
		PlayerCardOption1->GetUpgradeButton()->OnHovered.AddDynamic(this, &UUpgradeSelectionUI::TriggerHoverAnimForFirstOption);
		PlayerCardOption1->GetUpgradeButton()->OnClicked.AddDynamic(this, &UUpgradeSelectionUI::TriggerSelectAnimForFirstOption);
	}
	if (PlayerCardOption2)
	{
		PlayerCardOption2->GetUpgradeButton()->OnHovered.AddDynamic(this, &UUpgradeSelectionUI::TriggerHoverAnimForSecondOption);
		PlayerCardOption2->GetUpgradeButton()->OnClicked.AddDynamic(this, &UUpgradeSelectionUI::TriggerSelectAnimForSecondOption);
	}
	if (EnemyCardOption1)
	{
		EnemyCardOption1->GetUpgradeButton()->OnHovered.AddDynamic(this, &UUpgradeSelectionUI::TriggerHoverAnimForFirstOption);
		EnemyCardOption1->GetUpgradeButton()->OnClicked.AddDynamic(this, &UUpgradeSelectionUI::TriggerSelectAnimForFirstOption);
	}
	if (EnemyCardOption2)
	{
		EnemyCardOption2->GetUpgradeButton()->OnHovered.AddDynamic(this, &UUpgradeSelectionUI::TriggerHoverAnimForSecondOption);
		EnemyCardOption2->GetUpgradeButton()->OnClicked.AddDynamic(this, &UUpgradeSelectionUI::TriggerSelectAnimForSecondOption);
	}
}

void UUpgradeSelectionUI::TriggerUpgradeMenuCloseAnimation()
{
	if (UpgradeMenuClose)
	{
		PlayAnimation(UpgradeMenuClose);
		UKismetSystemLibrary::K2_SetTimer(this, "FinishedUpgradeSelectionCloseAnimation", UpgradeMenuClose->GetEndTime(), false);
	}
}

void UUpgradeSelectionUI::FinishedUpgradeSelectionCloseAnimation() const
{
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UUpgradeSelectionUI>(EWidget::UpgradeSelectionUI)->CloseUI();
	(void)DelegateManager->OnUpgradeSelectionFinished.ExecuteIfBound();
}

void UUpgradeSelectionUI::TriggerHoverAnimForFirstOption()
{
	if (PlayerCardOption1Popup && EnemyCardOption1Popup)
	{
		PlayAnimation(PlayerCardOption1Popup, GetAnimationCurrentTime(PlayerCardOption1Popup));
		PlayAnimation(EnemyCardOption1Popup, GetAnimationCurrentTime(EnemyCardOption1Popup));
	}
}

void UUpgradeSelectionUI::TriggerHoverAnimForSecondOption()
{
	if (PlayerCardOption2Popup && EnemyCardOption2Popup)
	{
		PlayAnimation(PlayerCardOption2Popup, GetAnimationCurrentTime(PlayerCardOption2Popup));
		PlayAnimation(EnemyCardOption2Popup, GetAnimationCurrentTime(EnemyCardOption2Popup));
	}
}

void UUpgradeSelectionUI::TriggerSelectAnimForFirstOption()
{
	if (PlayerCardOption1Select && EnemyCardOption1Select)
	{
		PlayAnimation(PlayerCardOption1Select);
		PlayAnimation(EnemyCardOption1Select);
	}
}

void UUpgradeSelectionUI::TriggerSelectAnimForSecondOption()
{
	if (PlayerCardOption2Select && EnemyCardOption2Select)
	{
		PlayAnimation(PlayerCardOption2Select);
		PlayAnimation(EnemyCardOption2Select);
	}
}
