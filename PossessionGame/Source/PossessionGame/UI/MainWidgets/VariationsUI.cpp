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
#include "PossessionGame/UI/SecondaryWidgets/CloseButtonUI.h"
#include "PossessionGame/UI/SecondaryWidgets/VariationCurrencyPanelUI.h"
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

void UVariationsUI::CloseUI() { Toggle(false); }

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
	GuardianVariationDefault->ShowDetailsPanel.BindUObject(this, &UVariationsUI::ToggleDetailsPanel);
	GuardianVariationGold->ShowDetailsPanel.BindUObject(this, &UVariationsUI::ToggleDetailsPanel);
	RangerVariationDefault->ShowDetailsPanel.BindUObject(this, &UVariationsUI::ToggleDetailsPanel);
	RangerVariationBlue->ShowDetailsPanel.BindUObject(this, &UVariationsUI::ToggleDetailsPanel);
	TricksterVariationDefault->ShowDetailsPanel.BindUObject(this, &UVariationsUI::ToggleDetailsPanel);
	TricksterVariationPurple->ShowDetailsPanel.BindUObject(this, &UVariationsUI::ToggleDetailsPanel);
	BreacherVariationDefault->ShowDetailsPanel.BindUObject(this, &UVariationsUI::ToggleDetailsPanel);
	WraithVariationDefault->ShowDetailsPanel.BindUObject(this, &UVariationsUI::ToggleDetailsPanel);
	TankVariationDefault->ShowDetailsPanel.BindUObject(this, &UVariationsUI::ToggleDetailsPanel);
	SpyVariationDefault->ShowDetailsPanel.BindUObject(this, &UVariationsUI::ToggleDetailsPanel);
	SamuraiVariationDefault->ShowDetailsPanel.BindUObject(this, &UVariationsUI::ToggleDetailsPanel);
	CloseButton->Get()->OnClicked.AddDynamic(this, &UVariationsUI::CloseUI);
	if (!GetGameInstance()->GetSubsystem<UDelegateManager>()->RequestIsDemo.Execute())
	{
		RightArrowButton->OnClicked.AddDynamic(this, &UVariationsUI::SwitchCharactersSelectionRight);
		RightArrowButton->OnHovered.AddDynamic(this, &UVariationsUI::RightArrowButtonHovered);
		RightArrowButton->OnUnhovered.AddDynamic(this, &UVariationsUI::RightArrowButtonUnHovered);
		LeftArrowButton->OnClicked.AddDynamic(this, &UVariationsUI::SwitchCharactersSelectionLeft);
		LeftArrowButton->OnHovered.AddDynamic(this, &UVariationsUI::LeftArrowButtonHovered);
		LeftArrowButton->OnUnhovered.AddDynamic(this, &UVariationsUI::LeftArrowButtonUnHovered);
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
	switch (InVariation)
	{
		case EVariation::GuardianVariationDefault: GuardianVariationDefault->SetSelected(true); break;
		case EVariation::GuardianVariationGold: GuardianVariationGold->SetSelected(true); break;
		case EVariation::RangerVariationDefault: RangerVariationDefault->SetSelected(true); break;
		case EVariation::RangerVariationBlue: RangerVariationBlue->SetSelected(true); break;
		case EVariation::TricksterVariationDefault: TricksterVariationDefault->SetSelected(true); break;
		case EVariation::TricksterVariationPurple: TricksterVariationPurple->SetSelected(true); break;
		case EVariation::BreacherVariationDefault: BreacherVariationDefault->SetSelected(true); break;
		case EVariation::WraithVariationDefault: WraithVariationDefault->SetSelected(true); break;
		case EVariation::TankVariationDefault: TankVariationDefault->SetSelected(true); break;
		case EVariation::SpyVariationDefault: SpyVariationDefault->SetSelected(true); break;
		case EVariation::SamuraiVariationDefault: SamuraiVariationDefault->SetSelected(true);
		default: ;
	}
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
			if (SaveSlot->GetIsVariationUnlocked(EVariation::GuardianVariationGold)) GuardianVariationGold->UnlockVariation();
			break;
		case ECharacterType::Ranger:
			CurrentlySelectedIcon = RangerCharacterIcon;
			ActiveWidgetIndex = 1;
			if (SaveSlot->GetIsVariationUnlocked(EVariation::RangerVariationBlue)) RangerVariationBlue->UnlockVariation();
			break;
		case ECharacterType::Trickster:
			CurrentlySelectedIcon = TricksterCharacterIcon;
			ActiveWidgetIndex = 2;
			if (SaveSlot->GetIsVariationUnlocked(EVariation::TricksterVariationPurple)) TricksterVariationPurple->UnlockVariation();
			break;
		case ECharacterType::Breacher:
			CurrentlySelectedIcon = BreacherCharacterIcon;
			ActiveWidgetIndex = 3;
		//	if (SaveSlot->GetIsVariationUnlocked(EVariation::TricksterVariationPurple)) TricksterVariationPurple->UnlockVariation();
			break;
		case ECharacterType::Wraith:
			CurrentlySelectedIcon = WraithCharacterIcon;
			ActiveWidgetIndex = 4;
		//	if (SaveSlot->GetIsVariationUnlocked(EVariation::TricksterVariationPurple)) TricksterVariationPurple->UnlockVariation();
			break;
		case ECharacterType::Tank:
			CurrentlySelectedIcon = TankCharacterIcon;
			ActiveWidgetIndex = 5;
		//	if (SaveSlot->GetIsVariationUnlocked(EVariation::TricksterVariationPurple)) TricksterVariationPurple->UnlockVariation();
			break;
		case ECharacterType::Spy:
			CurrentlySelectedIcon = SpyCharacterIcon;
			ActiveWidgetIndex = 6;
		//	if (SaveSlot->GetIsVariationUnlocked(EVariation::TricksterVariationPurple)) TricksterVariationPurple->UnlockVariation();
			break;
		case ECharacterType::Samurai:
			CurrentlySelectedIcon = SamuraiCharacterIcon;
			ActiveWidgetIndex = 7;
		//	if (SaveSlot->GetIsVariationUnlocked(EVariation::TricksterVariationPurple)) TricksterVariationPurple->UnlockVariation();
			break;
		default: ;
	}
	VariationsSwitcher->SetActiveWidgetIndex(ActiveWidgetIndex);
}

void UVariationsUI::ToggleDetailsPanel(const bool bState, const EVariation InVariationName, const FText& InVariationDetailsText, const FText& InUnlockDetailsText)
{
	if (!UnlockDetailsPanel || !VariationDetailsPanel) return;
	if (bState)
	{
		if (VariationDetailsPanel->GetVisibility() == ESlateVisibility::Hidden) VariationDetailsPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		if (UnlockDetailsPanel->GetVisibility() == ESlateVisibility::Hidden) UnlockDetailsPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		VariationDetailsPanel->SetDetailsText(InVariationDetailsText);
		UnlockDetailsPanel->SetDetailsText(InUnlockDetailsText);
		if (const int32* UnlockPrice = UnlockPrices.Find(InVariationName)) UnlockDetailsPanel->SetupDetailsPanel(InVariationName, *UnlockPrice);
		else UnlockDetailsPanel->SetupDetailsPanel(InVariationName, INT_MAX);
		if (SaveSlot->GetIsVariationUnlocked(InVariationName)) UnlockDetailsPanel->SetUnlockedVariation(true);
		else UnlockDetailsPanel->SetTaskCompleted(SaveSlot->GetIsVariationTaskCompleted(InVariationName));
		UnlockDetailsPanel->PlayFadeInAnim();
		VariationDetailsPanel->PlayFadeInAnim();
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
	PlayAnimation(ArrowRightPressAnim);
	int32 NextActiveIndex = CharactersSwitcher->GetActiveWidgetIndex();
	if (++NextActiveIndex >= CharactersSwitcher->GetNumWidgets()) NextActiveIndex = 0;
	CharactersSwitcher->SetActiveWidgetIndex(NextActiveIndex);
}

void UVariationsUI::SwitchCharactersSelectionLeft()
{
	if (!CharactersSwitcher) return;
	PlayAnimation(ArrowLeftPressAnim);
	int32 NextActiveIndex = CharactersSwitcher->GetActiveWidgetIndex();
	if (--NextActiveIndex < 0) NextActiveIndex = CharactersSwitcher->GetNumWidgets() - 1;
	CharactersSwitcher->SetActiveWidgetIndex(NextActiveIndex);
}

void UVariationsUI::LeftArrowButtonHovered()
{
	PlayAnimation(ArrowLeftHoverAnim, GetAnimationCurrentTime(ArrowLeftHoverAnim));
}

void UVariationsUI::LeftArrowButtonUnHovered()
{
	PlayAnimation(ArrowLeftHoverAnim, GetAnimationCurrentTime(ArrowLeftHoverAnim), 1, EUMGSequencePlayMode::Reverse);
}

void UVariationsUI::RightArrowButtonHovered()
{
	PlayAnimation(ArrowRightHoverAnim, GetAnimationCurrentTime(ArrowRightHoverAnim));
}

void UVariationsUI::RightArrowButtonUnHovered()
{
	PlayAnimation(ArrowRightHoverAnim, GetAnimationCurrentTime(ArrowRightHoverAnim), 1, EUMGSequencePlayMode::Reverse);
}
