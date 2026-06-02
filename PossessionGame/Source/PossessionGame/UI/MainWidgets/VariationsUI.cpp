#include "VariationsUI.h"
#include "PlayerHUD.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "PossessionGame/UI/SecondaryWidgets/VariationCharacterIconUI.h"
#include "PossessionGame/UI/SecondaryWidgets/VariationOptionUI.h"
#include "PossessionGame/UI/SecondaryWidgets/VariationUnlockDetailsUI.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGame/Enums/EExplanation.h"
#include "PossessionGame/Enums/EVariation.h"
#include "PossessionGame/UI/SecondaryWidgets/ArrowButtonUI.h"
#include "PossessionGame/UI/SecondaryWidgets/CloseButtonUI.h"
#include "PossessionGame/UI/SecondaryWidgets/VariationCurrencyPanelUI.h"
#include "PossessionGame/UI/SecondaryWidgets/VariationDescriptionUI.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/SaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"

void UVariationsUI::OpenUI()
{
	if (!bHasAlreadyOpened)
	{
		SetSelectedVariation(SaveSlot->GetSelectedVariation(ECharacterType::Guardian));
		SetSelectedVariation(SaveSlot->GetSelectedVariation(ECharacterType::Ranger));
		SetSelectedVariation(SaveSlot->GetSelectedVariation(ECharacterType::Trickster));
		SetSelectedVariation(SaveSlot->GetSelectedVariation(ECharacterType::Breacher));
		SetSelectedVariation(SaveSlot->GetSelectedVariation(ECharacterType::Wraith));
		SetSelectedVariation(SaveSlot->GetSelectedVariation(ECharacterType::Tank));
		SetSelectedVariation(SaveSlot->GetSelectedVariation(ECharacterType::Spy));
		SetSelectedVariation(SaveSlot->GetSelectedVariation(ECharacterType::Samurai));
		bHasAlreadyOpened = true;
	}
	Toggle(true);
}

void UVariationsUI::CloseUI()
{
	Toggle(false);
	(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->OnVariationUIClosed.ExecuteIfBound();
}

void UVariationsUI::NativeConstruct()
{
	Super::NativeConstruct();
	SaveSlot = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot();
	UnlockDetailsPanel->SetSaveSlot(SaveSlot);
	SwitchVariation(ECharacterType::Guardian);
	GuardianCharacterIcon->PlayHoverAnim();
	GuardianCharacterIcon->SetEventsEnabled(false);
	GuardianCharacterIcon->OnVariationClicked.BindUObject(this, &UVariationsUI::SwitchVariation);
	RangerCharacterIcon->OnVariationClicked.BindUObject(this, &UVariationsUI::SwitchVariation);
	TricksterCharacterIcon->OnVariationClicked.BindUObject(this, &UVariationsUI::SwitchVariation);
	BreacherCharacterIcon->OnVariationClicked.BindUObject(this, &UVariationsUI::SwitchVariation);
	WraithCharacterIcon->OnVariationClicked.BindUObject(this, &UVariationsUI::SwitchVariation);
	TankCharacterIcon->OnVariationClicked.BindUObject(this, &UVariationsUI::SwitchVariation);
	SpyCharacterIcon->OnVariationClicked.BindUObject(this, &UVariationsUI::SwitchVariation);
	SamuraiCharacterIcon->OnVariationClicked.BindUObject(this, &UVariationsUI::SwitchVariation);
	// Guardian
	VariationOptions.Add(EVariation::GuardianDefault, GuardianDefault);
	VariationOptions.Add(EVariation::GuardianGreen, GuardianGreen);
	VariationOptions.Add(EVariation::GuardianPurple, GuardianPurple);
	VariationOptions.Add(EVariation::GuardianBlack, GuardianBlack);
	// Ranger
	VariationOptions.Add(EVariation::RangerDefault, RangerDefault);
	VariationOptions.Add(EVariation::RangerBlue, RangerBlue);
	VariationOptions.Add(EVariation::RangerGreen, RangerGreen);
	VariationOptions.Add(EVariation::RangerBlack, RangerBlack);
	// Trickster
	VariationOptions.Add(EVariation::TricksterDefault, TricksterDefault);
	VariationOptions.Add(EVariation::TricksterPurple, TricksterPurple);
	VariationOptions.Add(EVariation::TricksterLime, TricksterLime);
	VariationOptions.Add(EVariation::TricksterBlack, TricksterBlack);
	// Breacher
	VariationOptions.Add(EVariation::BreacherDefault, BreacherDefault);
	VariationOptions.Add(EVariation::BreacherRed, BreacherRed);
	VariationOptions.Add(EVariation::BreacherGreen, BreacherGreen);
	VariationOptions.Add(EVariation::BreacherBlack, BreacherBlack);
	// Wraith
	VariationOptions.Add(EVariation::WraithDefault, WraithDefault);
	VariationOptions.Add(EVariation::WraithPurple, WraithPurple);
	VariationOptions.Add(EVariation::WraithBlack, WraithBlack);
	VariationOptions.Add(EVariation::WraithBlue, WraithBlue);
	// Tank
	VariationOptions.Add(EVariation::TankDefault, TankDefault);
	VariationOptions.Add(EVariation::TankBlack, TankBlack);
	VariationOptions.Add(EVariation::TankGreen, TankGreen);
	VariationOptions.Add(EVariation::TankOrange, TankOrange);
	// Spy
	VariationOptions.Add(EVariation::SpyDefault, SpyDefault);
	VariationOptions.Add(EVariation::SpyBlack, SpyBlack);
	VariationOptions.Add(EVariation::SpyOrange, SpyOrange);
	VariationOptions.Add(EVariation::SpyYellow, SpyYellow);
	// Samurai
	VariationOptions.Add(EVariation::SamuraiDefault, SamuraiDefault);
	VariationOptions.Add(EVariation::SamuraiBlack, SamuraiBlack);
	VariationOptions.Add(EVariation::SamuraiBlue, SamuraiBlue);
	VariationOptions.Add(EVariation::SamuraiRed, SamuraiRed);
	for (const TPair<EVariation, TObjectPtr<UVariationOptionUI>>& Pair : VariationOptions)
	{
		if (const TObjectPtr<UVariationOptionUI> Value = Pair.Value) Value->ShowDetailsPanel.BindUObject(this, &UVariationsUI::ToggleDetailsPanel);
	}
	CloseButton->Get()->OnClicked.AddDynamic(this, &UVariationsUI::CloseUI);
	if (!GetGameInstance()->GetSubsystem<UDelegateManager>()->RequestIsDemo.Execute())
	{
		RightArrowButton->GetClickEvent().AddDynamic(this, &UVariationsUI::SwitchCharactersSelectionRight);
		RightArrowButton->SetupEvents();
		LeftArrowButton->GetClickEvent().AddDynamic(this, &UVariationsUI::SwitchCharactersSelectionLeft);
		LeftArrowButton->SetupEvents();
		VariationPageRightArrowButton->GetClickEvent().AddDynamic(this, &UVariationsUI::SwitchVariationPageRight);
		VariationPageRightArrowButton->SetupEvents();
		VariationPageLeftArrowButton->GetClickEvent().AddDynamic(this, &UVariationsUI::SwitchVariationPageLeft);
		VariationPageLeftArrowButton->SetupEvents();
	}
	FadeOutDelegate.BindDynamic(this, &UVariationsUI::HideVariationsPanel);
}

void UVariationsUI::Toggle(const bool bState)
{
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UPlayerHUD>(EWidget::PlayerHUD)->PlayFadeOutAnim(bState);
	UGameplayStatics::SetGamePaused(GetWorld(), bState);
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(bState ? ESound::PanelFadeIn : ESound::PanelFadeOut);
	const TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->bShowMouseCursor = bState;
	GetWorld()->GetSubsystem<UWidgetManager>()->SetCurrentStateOfUI(bState ? EWidget::VariationsUI : EWidget::None);
	if (bState)
	{
		if (VariationCurrencyPanel) VariationCurrencyPanel->UpdateCurrencyText();
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
	if (const TObjectPtr<UVariationOptionUI>* Option = VariationOptions.Find(InVariation)) (*Option)->SetSelected(true);
}

void UVariationsUI::SwitchVariation(const ECharacterType InCharacterName)
{
	if (CurrentlySelectedIcon) CurrentlySelectedIcon->SetEventsEnabled(true);
	int32 ActiveWidgetIndex = 0;
	switch (InCharacterName)
	{
		case ECharacterType::Guardian:
			CurrentlySelectedIcon = GuardianCharacterIcon;
			ActiveWidgetIndex = 0;
			CurrentVariationsSwitcherSelected = GuardianVariations;
			if (SaveSlot->GetIsVariationUnlocked(EVariation::GuardianGreen)) GuardianGreen->UnlockVariation();
			if (SaveSlot->GetIsVariationUnlocked(EVariation::GuardianPurple)) GuardianPurple->UnlockVariation();
			if (SaveSlot->GetIsVariationUnlocked(EVariation::GuardianBlack)) GuardianBlack->UnlockVariation();
			break;
		case ECharacterType::Ranger:
			CurrentlySelectedIcon = RangerCharacterIcon;
			ActiveWidgetIndex = 1;
			CurrentVariationsSwitcherSelected = RangerVariations;
			if (SaveSlot->GetIsVariationUnlocked(EVariation::RangerBlue)) RangerBlue->UnlockVariation();
			if (SaveSlot->GetIsVariationUnlocked(EVariation::RangerGreen)) RangerGreen->UnlockVariation();
			if (SaveSlot->GetIsVariationUnlocked(EVariation::RangerBlack)) RangerBlack->UnlockVariation();
			break;
		case ECharacterType::Trickster:
			CurrentlySelectedIcon = TricksterCharacterIcon;
			ActiveWidgetIndex = 2;
			CurrentVariationsSwitcherSelected = TricksterVariations;
			if (SaveSlot->GetIsVariationUnlocked(EVariation::TricksterPurple)) TricksterPurple->UnlockVariation();
			if (SaveSlot->GetIsVariationUnlocked(EVariation::TricksterLime)) TricksterLime->UnlockVariation();
			if (SaveSlot->GetIsVariationUnlocked(EVariation::TricksterBlack)) TricksterBlack->UnlockVariation();
			break;
		case ECharacterType::Breacher:
			CurrentlySelectedIcon = BreacherCharacterIcon;
			ActiveWidgetIndex = 3;
			CurrentVariationsSwitcherSelected = BreacherVariations;
			if (SaveSlot->GetIsVariationUnlocked(EVariation::BreacherRed)) BreacherRed->UnlockVariation();
			if (SaveSlot->GetIsVariationUnlocked(EVariation::BreacherGreen)) BreacherGreen->UnlockVariation();
			if (SaveSlot->GetIsVariationUnlocked(EVariation::BreacherBlack)) BreacherBlack->UnlockVariation();
			break;
		case ECharacterType::Wraith:
			CurrentlySelectedIcon = WraithCharacterIcon;
			ActiveWidgetIndex = 4;
			CurrentVariationsSwitcherSelected = WraithVariations;
			if (SaveSlot->GetIsVariationUnlocked(EVariation::WraithPurple)) WraithPurple->UnlockVariation();
			if (SaveSlot->GetIsVariationUnlocked(EVariation::WraithBlue)) WraithBlue->UnlockVariation();
			if (SaveSlot->GetIsVariationUnlocked(EVariation::WraithBlack)) WraithBlack->UnlockVariation();
			break;
		case ECharacterType::Spy:
			CurrentlySelectedIcon = SpyCharacterIcon;
			ActiveWidgetIndex = 5;
			CurrentVariationsSwitcherSelected = SpyVariations;
			if (SaveSlot->GetIsVariationUnlocked(EVariation::SpyOrange)) SpyOrange->UnlockVariation();
			if (SaveSlot->GetIsVariationUnlocked(EVariation::SpyYellow)) SpyYellow->UnlockVariation();
			if (SaveSlot->GetIsVariationUnlocked(EVariation::SpyBlack)) SpyBlack->UnlockVariation();
			break;
		case ECharacterType::Tank:
			CurrentlySelectedIcon = TankCharacterIcon;
			ActiveWidgetIndex = 6;
			CurrentVariationsSwitcherSelected = TankVariations;
			if (SaveSlot->GetIsVariationUnlocked(EVariation::TankGreen)) TankGreen->UnlockVariation();
			if (SaveSlot->GetIsVariationUnlocked(EVariation::TankOrange)) TankOrange->UnlockVariation();
			if (SaveSlot->GetIsVariationUnlocked(EVariation::TankBlack)) TankBlack->UnlockVariation();
			break;
		case ECharacterType::Samurai:
			CurrentlySelectedIcon = SamuraiCharacterIcon;
			ActiveWidgetIndex = 7;
			CurrentVariationsSwitcherSelected = SamuraiVariations;
			if (SaveSlot->GetIsVariationUnlocked(EVariation::SamuraiBlue)) SamuraiBlue->UnlockVariation();
			if (SaveSlot->GetIsVariationUnlocked(EVariation::SamuraiRed)) SamuraiRed->UnlockVariation();
			if (SaveSlot->GetIsVariationUnlocked(EVariation::SamuraiBlack)) SamuraiBlack->UnlockVariation();
			break;
		default: ;
	}
	VariationsSwitcher->SetActiveWidgetIndex(ActiveWidgetIndex);
}

void UVariationsUI::ToggleDetailsPanel(const bool bState, const EVariation InVariationName, const FText& InVariationDetailsText, const FText& InUnlockDetailsText)
{
	if (!UnlockDetailsPanel || !VariationDescriptionPanel) return;
	if (bState)
	{
		if (VariationDescriptionPanel->GetVisibility() == ESlateVisibility::Hidden) VariationDescriptionPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		if (UnlockDetailsPanel->GetVisibility() == ESlateVisibility::Hidden) UnlockDetailsPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		VariationDescriptionPanel->SetDetailsText(InVariationDetailsText);
		UnlockDetailsPanel->SetDetailsText(InUnlockDetailsText);
		if (const int32* UnlockPrice = UnlockPrices.Find(InVariationName)) UnlockDetailsPanel->SetupDetailsPanel(InVariationName, *UnlockPrice);
		else UnlockDetailsPanel->SetupDetailsPanel(InVariationName, INT_MAX);
		UnlockDetailsPanel->SetUnlockedVariation(SaveSlot->GetIsVariationUnlocked(InVariationName));
		UnlockDetailsPanel->PlayFadeInAnim();
		VariationDescriptionPanel->PlayFadeInAnim();
		if (!GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetIsExplanationShown(EExplanation::VariationDetailsUI))
		{
			(void)GetWorld()->GetSubsystem<UWidgetManager>()->GetOnShowExplanationDelegate().ExecuteIfBound(EExplanation::VariationDetailsUI, GetGameInstance());
		}
	}
}

void UVariationsUI::HideVariationsPanel()
{
	UnbindAllFromAnimationFinished(VariationMenuFadeInAnim);
	SetVisibility(ESlateVisibility::Collapsed);
}

void UVariationsUI::SwitchCharactersSelectionRight()
{
	if (!CharactersSwitcher) return;
	int32 NextActiveIndex = CharactersSwitcher->GetActiveWidgetIndex();
	if (++NextActiveIndex >= CharactersSwitcher->GetNumWidgets()) NextActiveIndex = 0;
	CharactersSwitcher->SetActiveWidgetIndex(NextActiveIndex);
}

void UVariationsUI::SwitchCharactersSelectionLeft()
{
	if (!CharactersSwitcher) return;
	int32 NextActiveIndex = CharactersSwitcher->GetActiveWidgetIndex();
	if (--NextActiveIndex < 0) NextActiveIndex = CharactersSwitcher->GetNumWidgets() - 1;
	CharactersSwitcher->SetActiveWidgetIndex(NextActiveIndex);
}

void UVariationsUI::SwitchVariationPageRight()
{
	if (!CurrentVariationsSwitcherSelected) return;
	int32 NextActiveIndex = CurrentVariationsSwitcherSelected->GetActiveWidgetIndex();
	if (++NextActiveIndex >= CurrentVariationsSwitcherSelected->GetNumWidgets()) NextActiveIndex = 0;
	CurrentVariationsSwitcherSelected->SetActiveWidgetIndex(NextActiveIndex);
}

void UVariationsUI::SwitchVariationPageLeft()
{
	if (!CurrentVariationsSwitcherSelected) return;
	int32 NextActiveIndex = CurrentVariationsSwitcherSelected->GetActiveWidgetIndex();
	if (--NextActiveIndex < 0) NextActiveIndex = CurrentVariationsSwitcherSelected->GetNumWidgets() - 1;
	CurrentVariationsSwitcherSelected->SetActiveWidgetIndex(NextActiveIndex);
}
