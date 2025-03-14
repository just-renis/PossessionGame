#include "UpgradeSelectionUI.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Data Assets/Upgrades/UpgradeData.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EWidget.h"
#include "PossessionGame/UI/SecondaryWidgets/UpgradeCardUI.h"
#include "PossessionGame/Utilities/Subsystems/UpgradeManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void UUpgradeSelectionUI::OpenUI()
{
	Toggle(true);
	const TArray<TObjectPtr<UUpgradeData>> Data = GetWorld()->GetSubsystem<UUpgradeManager>()->GetRandomUpgrades();
	if (Data.Num() > 0)
	{
		PlayerCardOption1->SetupCard(true, PlayerCardBackgroundTexture, Data[0], Data[0]->GetCharacter() == ECharacterType::Ranger ? RangerIconTexture : TricksterIconTexture);
		EnemyCardOption1->SetupCard(false, EnemyCardBackgroundTexture, Data[0]);
	}
	if (Data.Num() > 1)
	{
		PlayerCardOption2->SetupCard(true, PlayerCardBackgroundTexture, Data[1], Data[1]->GetCharacter() == ECharacterType::Ranger ? RangerIconTexture : TricksterIconTexture);
		EnemyCardOption2->SetupCard(false, EnemyCardBackgroundTexture, Data[1]);
	}
}

void UUpgradeSelectionUI::CloseUI()
{
	PlayerCardOption1->ToggleCardVisibility(false);
	EnemyCardOption1->ToggleCardVisibility(false);
	PlayerCardOption2->ToggleCardVisibility(false);
	EnemyCardOption2->ToggleCardVisibility(false);
	Toggle(false);
}

void UUpgradeSelectionUI::Toggle(const bool bState)
{
	UGameplayStatics::SetGamePaused(GetWorld(), bState);
	SetVisibility(bState ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	const TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->bShowMouseCursor = bState;
	if (bState) PlayerController->SetInputMode(FInputModeGameAndUI());
	else PlayerController->SetInputMode(FInputModeGameOnly());
	GetWorld()->GetSubsystem<UWidgetManager>()->SetCurrentStateOfUI(bState ? EWidget::UpgradeSelectionUI : EWidget::None);
}
