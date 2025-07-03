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
	}
	Toggle(true);
}

void UVariationsUI::CloseUI() { Toggle(false); }

void UVariationsUI::NativeConstruct()
{
	Super::NativeConstruct();
	SwitchVariation(0);
	GuardianCharacterIcon->PlayHoverAnim();
	GuardianCharacterIcon->OnVariationClicked.BindUObject(this, &UVariationsUI::SwitchVariation);
	RangerCharacterIcon->OnVariationClicked.BindUObject(this, &UVariationsUI::SwitchVariation);
	TricksterCharacterIcon->OnVariationClicked.BindUObject(this, &UVariationsUI::SwitchVariation);
	GuardianVariationDefault->ShowDetailsPanel.BindUObject(this, &UVariationsUI::ToggleDetailsPanel);
	GuardianVariationGold->ShowDetailsPanel.BindUObject(this, &UVariationsUI::ToggleDetailsPanel);
	RangerVariationDefault->ShowDetailsPanel.BindUObject(this, &UVariationsUI::ToggleDetailsPanel);
	RangerVariationBlue->ShowDetailsPanel.BindUObject(this, &UVariationsUI::ToggleDetailsPanel);
	TricksterVariationDefault->ShowDetailsPanel.BindUObject(this, &UVariationsUI::ToggleDetailsPanel);
	TricksterVariationPurple->ShowDetailsPanel.BindUObject(this, &UVariationsUI::ToggleDetailsPanel);
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
		PlayerController->SetInputMode(FInputModeGameAndUI());
	}
	else
	{
		PlayerController->SetInputMode(FInputModeGameOnly());
		BindToAnimationFinished(VariationMenuFadeInAnim, FadeOutDelegate);
	}
	PlayAnimation(VariationMenuFadeInAnim, GetAnimationCurrentTime(VariationMenuFadeInAnim), 1, bState ? EUMGSequencePlayMode::Forward : EUMGSequencePlayMode::Reverse);
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
	if (CurrentlySelectedIcon) CurrentlySelectedIcon->SetEventsEnabled(true);
	const TObjectPtr<USaveSlot> SaveSlot = Cast<UPossessionGameInstance>(GetGameInstance())->GetSaveSlot();
	VariationsSwitcher->SetActiveWidgetIndex(InIndex);
	switch (InIndex)
	{
		case 0:
			CurrentlySelectedIcon = GuardianCharacterIcon;
			if (SaveSlot->GetIsVariationUnlocked(EVariation::GuardianVariationGold)) GuardianVariationGold->UnlockVariation();
			break;
		case 1:
			CurrentlySelectedIcon = RangerCharacterIcon;
			if (SaveSlot->GetIsVariationUnlocked(EVariation::RangerVariationBlue)) RangerVariationBlue->UnlockVariation();
			break;
		case 2:
			CurrentlySelectedIcon = TricksterCharacterIcon;
			if (SaveSlot->GetIsVariationUnlocked(EVariation::TricksterVariationPurple)) TricksterVariationPurple->UnlockVariation();
		default: ;
	}
}

void UVariationsUI::ToggleDetailsPanel(const bool bState, const EVariation InVariationName, const FString& InVariationDetailsText, const FString& InUnlockDetailsText)
{
	if (!UnlockDetailsPanel || !VariationDetailsPanel) return;
	if (bState)
	{
		UnbindAllFromAnimationFinished(DetailPanelFadeInAnim);
		VariationDetailsPanel->SetDetailsText(FText::FromString(InVariationDetailsText));
		UnlockDetailsPanel->SetDetailsText(FText::FromString(InUnlockDetailsText));
		const TObjectPtr<USaveSlot> SaveSlot = Cast<UPossessionGameInstance>(GetGameInstance())->GetSaveSlot();
		if (SaveSlot->GetIsVariationUnlocked(InVariationName)) UnlockDetailsPanel->SetUnlockedVariation(true);
		else UnlockDetailsPanel->SetTaskCompleted(SaveSlot->GetIsVariationTaskCompleted(InVariationName));
	}
	else BindToAnimationFinished(DetailPanelFadeInAnim, DetailsPanelFadeOutDelegate);
	PlayAnimation(DetailPanelFadeInAnim, GetAnimationCurrentTime(DetailPanelFadeInAnim), 1, bState ? EUMGSequencePlayMode::Forward : EUMGSequencePlayMode::Reverse);
}

void UVariationsUI::HideDetailsPanel() { UnbindAllFromAnimationFinished(DetailPanelFadeInAnim); }

void UVariationsUI::HideVariationsPanel() { UnbindAllFromAnimationFinished(VariationMenuFadeInAnim); }
