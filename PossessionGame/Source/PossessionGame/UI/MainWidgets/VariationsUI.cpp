#include "VariationsUI.h"
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
	return FString::Printf(TEXT(" [%d / %d]"), InSaveSlot->GetVariationUnlockProgress(InVariationUnlockName), InSaveSlot->GetVariationUnlockRequirement(InVariationUnlockName));
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
	GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(bState ? ESound::PanelFadeIn : ESound::PanelFadeOut);
	const TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->bShowMouseCursor = bState;
	GetWorld()->GetSubsystem<UWidgetManager>()->SetCurrentStateOfUI(bState ? EWidget::VariationsUI : EWidget::None);
	if (bState)
	{
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		UpdateVariationUnlocksProgress();
		PlayerController->SetInputMode(FInputModeGameAndUI());
	}
	else
	{
		PlayerController->SetInputMode(FInputModeGameOnly());
		UKismetSystemLibrary::K2_ClearTimer(this, "HideDetailsPanel");
		if (VariationMenuFadeInAnim) UKismetSystemLibrary::K2_SetTimer(this, "HideVariationsPanel", VariationMenuFadeInAnim->GetEndTime(), false);
	}
	if (!VariationMenuFadeInAnim) return;
	PlayAnimation(VariationMenuFadeInAnim, GetAnimationCurrentTime(VariationMenuFadeInAnim), 1, bState ? EUMGSequencePlayMode::Forward : EUMGSequencePlayMode::Reverse);
}

void UVariationsUI::UpdateVariationUnlocksProgress()
{
	const TObjectPtr<USaveSlot> SaveSlot = Cast<UPossessionGameInstance>(GetGameInstance())->GetSaveSlot();
	if (GuardianVariationGold && GuardianVariationGold->GetIsLocked())
	{
		if (SaveSlot->GetIsVariationUnlocked(EVariationUnlock::KillsWithGuardian)) GuardianVariationGold->UnlockVariation();
		else GuardianVariationGold->UpdateUnlockProgressText(GetUnlockProgressString(SaveSlot, EVariationUnlock::KillsWithGuardian));
	}
	if (RangerVariationBlue && RangerVariationBlue->GetIsLocked())
	{
		if (SaveSlot->GetIsVariationUnlocked(EVariationUnlock::KillsWithRangerGrenade)) RangerVariationBlue->UnlockVariation();
		else RangerVariationBlue->UpdateUnlockProgressText(GetUnlockProgressString(SaveSlot, EVariationUnlock::KillsWithRangerGrenade));
	}
	if (TricksterVariationPurple && TricksterVariationPurple->GetIsLocked())
	{
		if (SaveSlot->GetIsVariationUnlocked(EVariationUnlock::DamageDealtWithTricksterPoisonGrenade)) TricksterVariationPurple->UnlockVariation();
		else TricksterVariationPurple->UpdateUnlockProgressText(GetUnlockProgressString(SaveSlot, EVariationUnlock::DamageDealtWithTricksterPoisonGrenade));
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

void UVariationsUI::SwitchVariation(const int32 InIndex)
{
	VariationsSwitcher->SetActiveWidgetIndex(InIndex);
	switch (CurrentCharacterSelected)
	{
		case 0: GuardianCharacterIcon->SetEventsEnabled(true); break;
		case 1: RangerCharacterIcon->SetEventsEnabled(true); break;
		case 2: TricksterCharacterIcon->SetEventsEnabled(true);
		default: ;
	}
	CurrentCharacterSelected = InIndex;
}

void UVariationsUI::ToggleDetailsPanel(const bool bState, const FString& InDetailsText)
{
	if (!UnlockDetailsPanel) return;
	if (bState)
	{
		UnlockDetailsPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		UnlockDetailsPanel->SetDetailsText(FText::FromString(InDetailsText));
	}
	if (!DetailPanelFadeInAnim) return;
	UKismetSystemLibrary::K2_SetTimer(this, "HideDetailsPanel", DetailPanelFadeInAnim->GetEndTime(), false);
	PlayAnimation(DetailPanelFadeInAnim, GetAnimationCurrentTime(DetailPanelFadeInAnim), 1, bState ? EUMGSequencePlayMode::Forward : EUMGSequencePlayMode::Reverse);
}

void UVariationsUI::HideDetailsPanel() const { if (UnlockDetailsPanel) UnlockDetailsPanel->SetVisibility(ESlateVisibility::Hidden); }

void UVariationsUI::HideVariationsPanel() { if (this) SetVisibility(ESlateVisibility::Collapsed); }
