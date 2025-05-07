#include "VariationsUI.h"
#include "Components/WidgetSwitcher.h"
#include "PossessionGame/UI/SecondaryWidgets/VariationCharacterIconUI.h"
#include "PossessionGame/UI/SecondaryWidgets/VariationOptionUI.h"
#include "PossessionGame/UI/SecondaryWidgets/VariationUnlockDetailsUI.h"
#include "PossessionGame/Utilities/Data Utilities/SaveSlot.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameInstance.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"
#include "Kismet/GameplayStatics.h"

void UVariationsUI::OpenUI() { Toggle(true); }

void UVariationsUI::CloseUI() { Toggle(false); }

FString UVariationsUI::GetUnlockProgressString(const TObjectPtr<USaveSlot>& InSaveSlot, const EVariationUnlock InVariationUnlockName)
{
	return FString::Printf(TEXT("[%d / %d]"), InSaveSlot->GetVariationUnlockProgress(InVariationUnlockName), InSaveSlot->GetVariationUnlockRequirement(InVariationUnlockName));
}

void UVariationsUI::NativeConstruct()
{
	Super::NativeConstruct();
	if (GuardianCharacterIcon) GuardianCharacterIcon->OnVariationClicked.BindUObject(this, &UVariationsUI::SwitchVariation);
	if (RangerCharacterIcon) RangerCharacterIcon->OnVariationClicked.BindUObject(this, &UVariationsUI::SwitchVariation);
	if (TricksterCharacterIcon) TricksterCharacterIcon->OnVariationClicked.BindUObject(this, &UVariationsUI::SwitchVariation);
	if (GuardianVariationDefault) GuardianVariationDefault->ShowDetailsPanel.BindUObject(this, &UVariationsUI::ToggleDetailsPanel);
	if (GuardianVariationGold) GuardianVariationGold->ShowDetailsPanel.BindUObject(this, &UVariationsUI::ToggleDetailsPanel);
	if (RangerVariationBlue) RangerVariationBlue->ShowDetailsPanel.BindUObject(this, &UVariationsUI::ToggleDetailsPanel);
	if (TricksterVariationPurple) TricksterVariationPurple->ShowDetailsPanel.BindUObject(this, &UVariationsUI::ToggleDetailsPanel);
}

void UVariationsUI::Toggle(const bool bState)
{
	UGameplayStatics::SetGamePaused(GetWorld(), bState);
	SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	const TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->bShowMouseCursor = bState;
	if (bState)
	{
		UpdateVariationUnlocksProgress();
		PlayerController->SetInputMode(FInputModeGameAndUI());
	}
	else PlayerController->SetInputMode(FInputModeGameOnly());
	GetWorld()->GetSubsystem<UWidgetManager>()->SetCurrentStateOfUI(bState ? EWidget::VariationsUI : EWidget::None);
}

void UVariationsUI::UpdateVariationUnlocksProgress()
{
	const TObjectPtr<USaveSlot> SaveSlot = Cast<UPossessionGameInstance>(GetGameInstance())->GetSaveSlot();
	if (GuardianVariationGold) GuardianVariationGold->UpdateUnlockProgressText(GetUnlockProgressString(SaveSlot, EVariationUnlock::KillsWithGuardian));
	if (RangerVariationBlue) RangerVariationBlue->UpdateUnlockProgressText(GetUnlockProgressString(SaveSlot, EVariationUnlock::KillsWithRangerGrenade));
	if (TricksterVariationPurple) TricksterVariationPurple->UpdateUnlockProgressText(GetUnlockProgressString(SaveSlot, EVariationUnlock::DamageDealtWithTricksterPoisonGrenade));
}

void UVariationsUI::SwitchVariation(const int32 InIndex) const { VariationsSwitcher->SetActiveWidgetIndex(InIndex); }

void UVariationsUI::ToggleDetailsPanel(const bool bState, const FString& InDetailsText) const
{
	if (!UnlockDetailsPanel) return;
	UnlockDetailsPanel->SetVisibility(bState ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
	UnlockDetailsPanel->SetDetailsText(FText::FromString(InDetailsText));
}
