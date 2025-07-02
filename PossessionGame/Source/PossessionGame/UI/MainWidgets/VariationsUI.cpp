#include "VariationsUI.h"
#include "PlayerHUD.h"
#include "Animation/WidgetAnimation.h"
#include "Components/WidgetSwitcher.h"
#include "PossessionGame/UI/SecondaryWidgets/VariationCharacterIconUI.h"
#include "PossessionGame/UI/SecondaryWidgets/VariationOptionUI.h"
#include "PossessionGame/UI/SecondaryWidgets/VariationUnlockDetailsUI.h"
#include "PossessionGame/Utilities/Data Utilities/SaveSlot.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameInstance.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EVariation.h"
#include "PossessionGame/Enums/EVariationUnlock.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

void UVariationsUI::OpenUI()
{
	if (!bHasAlreadyOpened)
	{
		const TObjectPtr<USaveSlot> SaveSlot = Cast<UPossessionGameInstance>(GetGameInstance())->GetSaveSlot();
		SetSelectedVariation(SaveSlot->GetSelectedVariation(ECharacterType::Guardian));
		SetSelectedVariation(SaveSlot->GetSelectedVariation(ECharacterType::Ranger));
		SetSelectedVariation(SaveSlot->GetSelectedVariation(ECharacterType::Trickster));
		bHasAlreadyOpened = true;
		GuardianCharacterIcon->SetEventsEnabled(false);
		GuardianCharacterIcon->PlaySelectedAnim();
	}
	Toggle(true);
}

void UVariationsUI::CloseUI() { Toggle(false); }

FString UVariationsUI::GetUnlockProgressString(const TObjectPtr<USaveSlot>& InSaveSlot, const EVariationUnlock InVariationUnlockName)
{
	return TEXT(" [") + FString::FromInt(InSaveSlot->GetVariationUnlockProgress(InVariationUnlockName)) + TEXT(" / ") + FString::FromInt(InSaveSlot->GetVariationUnlockRequirement(InVariationUnlockName)) + TEXT("]");
}

void UVariationsUI::NativeConstruct()
{
	Super::NativeConstruct();
	if (GuardianCharacterIcon) GuardianCharacterIcon->OnVariationClicked.BindUObject(this, &UVariationsUI::SwitchVariation);
	if (RangerCharacterIcon) RangerCharacterIcon->OnVariationClicked.BindUObject(this, &UVariationsUI::SwitchVariation);
	if (TricksterCharacterIcon) TricksterCharacterIcon->OnVariationClicked.BindUObject(this, &UVariationsUI::SwitchVariation);
	if (GuardianVariationDefault) GuardianVariationDefault->ShowDetailsPanel.BindUObject(this, &UVariationsUI::ToggleDetailsPanel);
	if (GuardianVariationGold) GuardianVariationGold->ShowDetailsPanel.BindUObject(this, &UVariationsUI::ToggleDetailsPanel);
	if (RangerVariationDefault) RangerVariationDefault->ShowDetailsPanel.BindUObject(this, &UVariationsUI::ToggleDetailsPanel);
	if (RangerVariationBlue) RangerVariationBlue->ShowDetailsPanel.BindUObject(this, &UVariationsUI::ToggleDetailsPanel);
	if (TricksterVariationDefault) TricksterVariationDefault->ShowDetailsPanel.BindUObject(this, &UVariationsUI::ToggleDetailsPanel);
	if (TricksterVariationPurple) TricksterVariationPurple->ShowDetailsPanel.BindUObject(this, &UVariationsUI::ToggleDetailsPanel);
	FadeOutDelegate.BindDynamic(this, &UVariationsUI::HideVariationsPanel);
	DetailsPanelFadeOutDelegate.BindDynamic(this, &UVariationsUI::HideDetailsPanel);
}

void UVariationsUI::Toggle(const bool bState)
{
	SetIsEnabled(bState);
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UPlayerHUD>(EWidget::PlayerHUD)->PlayFadeOutAnim(bState);
	UGameplayStatics::SetGamePaused(GetWorld(), bState);
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(bState ? ESound::PanelFadeIn : ESound::PanelFadeOut);
	const TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->bShowMouseCursor = bState;
	GetWorld()->GetSubsystem<UWidgetManager>()->SetCurrentStateOfUI(bState ? EWidget::VariationsUI : EWidget::None);
	SetButtonStatesOnToggle(bState);
	if (bState)
	{
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		UpdateVariationUnlocksProgress();
		PlayerController->SetInputMode(FInputModeGameAndUI());
	}
	else
	{
		PlayerController->SetInputMode(FInputModeGameOnly());
		BindToAnimationFinished(VariationMenuFadeInAnim, FadeOutDelegate);
	}
	PlayAnimation(VariationMenuFadeInAnim, GetAnimationCurrentTime(VariationMenuFadeInAnim), 1, bState ? EUMGSequencePlayMode::Forward : EUMGSequencePlayMode::Reverse);
}

void UVariationsUI::UpdateVariationUnlocksProgress()
{
	const TObjectPtr<USaveSlot> SaveSlot = Cast<UPossessionGameInstance>(GetGameInstance())->GetSaveSlot();
	if (GuardianVariationGold)
	{
		if (SaveSlot->GetIsVariationUnlocked(EVariationUnlock::KillsWithGuardian)) GuardianVariationGold->UnlockVariation();
		GuardianVariationGold->UpdateUnlockProgressText(GetUnlockProgressString(SaveSlot, EVariationUnlock::KillsWithGuardian));
	}
	if (RangerVariationBlue)
	{
		if (SaveSlot->GetIsVariationUnlocked(EVariationUnlock::KillsWithRangerGrenade)) RangerVariationBlue->UnlockVariation();
		RangerVariationBlue->UpdateUnlockProgressText(GetUnlockProgressString(SaveSlot, EVariationUnlock::KillsWithRangerGrenade));
	}
	if (TricksterVariationPurple)
	{
		if (SaveSlot->GetIsVariationUnlocked(EVariationUnlock::DamageDealtWithTricksterPoisonGrenade)) TricksterVariationPurple->UnlockVariation();
		TricksterVariationPurple->UpdateUnlockProgressText(GetUnlockProgressString(SaveSlot, EVariationUnlock::DamageDealtWithTricksterPoisonGrenade));
	}
}

void UVariationsUI::SetSelectedVariation(const EVariation InVariation)
{
	switch (InVariation)
	{
		case EVariation::GuardianVariationDefault: GuardianVariationDefault->SetSelected(true); break;
		case EVariation::GuardianVariationGold: GuardianVariationGold->SetSelected(true); break;
		case EVariation::RangerVariationDefault: RangerVariationDefault->SetSelected(true); break;
		case EVariation::RangerVariationBlue: RangerVariationBlue->SetSelected(true); break;
		case EVariation::TricksterVariationDefault: TricksterVariationDefault->SetSelected(true); break;
		case EVariation::TricksterVariationPurple: TricksterVariationPurple->SetSelected(true);
		default: ;
	}
}

void UVariationsUI::SetButtonStatesOnToggle(const bool bState) const
{
	if (GuardianCharacterIcon) GuardianCharacterIcon->SetButtonState(bState);
	if (RangerCharacterIcon) RangerCharacterIcon->SetButtonState(bState);
	if (TricksterCharacterIcon) TricksterCharacterIcon->SetButtonState(bState);
	if (GuardianVariationDefault) GuardianVariationDefault->SetButtonState(bState);
	if (GuardianVariationGold) GuardianVariationGold->SetButtonState(bState);
	if (RangerVariationDefault) RangerVariationDefault->SetButtonState(bState);
	if (RangerVariationBlue) RangerVariationBlue->SetButtonState(bState);
	if (TricksterVariationDefault) TricksterVariationDefault->SetButtonState(bState);
	if (TricksterVariationPurple) TricksterVariationPurple->SetButtonState(bState);
}

void UVariationsUI::SwitchVariation(const int32 InIndex)
{
	VariationsSwitcher->SetActiveWidgetIndex(InIndex);
	switch (CurrentCharacterSelected)
	{
		case 0: if (GuardianCharacterIcon) GuardianCharacterIcon->SetEventsEnabled(true); break;
		case 1: if (RangerCharacterIcon) RangerCharacterIcon->SetEventsEnabled(true); break;
		case 2: if (TricksterCharacterIcon) TricksterCharacterIcon->SetEventsEnabled(true);
		default: ;
	}
	CurrentCharacterSelected = InIndex;
}

void UVariationsUI::ToggleDetailsPanel(const bool bState, const FString& InVariationDetailsText, const FString& InUnlockDetailsText)
{
	if (!UnlockDetailsPanel || !VariationDetailsPanel) return;
	if (bState)
	{
		UnbindAllFromAnimationFinished(DetailPanelFadeInAnim);
		VariationDetailsPanel->SetDetailsText(FText::FromString(InVariationDetailsText));
		VariationDetailsPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		UnlockDetailsPanel->SetDetailsText(FText::FromString(InUnlockDetailsText));
		UnlockDetailsPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else BindToAnimationFinished(DetailPanelFadeInAnim, DetailsPanelFadeOutDelegate);
	PlayAnimation(DetailPanelFadeInAnim, GetAnimationCurrentTime(DetailPanelFadeInAnim), 1, bState ? EUMGSequencePlayMode::Forward : EUMGSequencePlayMode::Reverse);
}

void UVariationsUI::HideDetailsPanel()
{
	UnbindAllFromAnimationFinished(DetailPanelFadeInAnim);
	if (UnlockDetailsPanel) UnlockDetailsPanel->SetVisibility(ESlateVisibility::Hidden);
	if (VariationDetailsPanel) VariationDetailsPanel->SetVisibility(ESlateVisibility::Hidden);
}

void UVariationsUI::HideVariationsPanel() { UnbindAllFromAnimationFinished(VariationMenuFadeInAnim); }
