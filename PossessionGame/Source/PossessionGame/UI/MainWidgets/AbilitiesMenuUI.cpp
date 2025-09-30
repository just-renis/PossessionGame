#include "AbilitiesMenuUI.h"
#include "PlayerHUD.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Enums/EExplanation.h"
#include "PossessionGame/UI/SecondaryWidgets/AbilityResetButtonUI.h"
#include "PossessionGame/UI/SecondaryWidgets/CharacterAbilitiesUI.h"
#include "PossessionGame/UI/SecondaryWidgets/CloseButtonUI.h"
#include "PossessionGame/UI/SecondaryWidgets/ConfirmationPopupUI.h"
#include "PossessionGame/UI/SecondaryWidgets/CustomTextUI.h"
#include "PossessionGame/UI/SecondaryWidgets/MainMenuButtonUI.h"
#include "PossessionGame/UI/SecondaryWidgets/VariationCharacterIconUI.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/SaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

TObjectPtr<UTexture2D> UAbilitiesMenuUI::GetAbilityIcon(const ECharacterType InCharacter, const int32 InAbilityIndex) const
{
	switch (InCharacter)
	{
		case ECharacterType::Guardian: return GuardianCharacterAbilities->GetAbilityIcon(InAbilityIndex);
		case ECharacterType::Ranger: return RangerCharacterAbilities->GetAbilityIcon(InAbilityIndex);
		case ECharacterType::Trickster: return TricksterCharacterAbilities->GetAbilityIcon(InAbilityIndex); 
		default: ;
	}
	return nullptr;
}

void UAbilitiesMenuUI::ToggleDetailsPanel(const bool bState, const FText& InText) const
{
	if (!DetailsPanel) return;
	DetailsPanel->SetText(InText);
	DetailsPanel->PlayFadeInAnim(bState);
}

void UAbilitiesMenuUI::OpenUI()
{
	UpdateSkillPointsText();
	Toggle(true);
	if (!FirstTimeOpenInstance) return;
	const TObjectPtr<USaveSlot> SaveSlot = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot();
	if (!SaveSlot->GetIsExplanationShown(EExplanation::AbilitiesUI))
	{
		(void)GetWorld()->GetSubsystem<UWidgetManager>()->GetOnShowExplanationDelegate().ExecuteIfBound(EExplanation::AbilitiesUI, GetGameInstance());
	}
	GuardianCharacterAbilities->LoadAbilitiesLayout(SaveSlot);
	RangerCharacterAbilities->LoadAbilitiesLayout(SaveSlot);
	TricksterCharacterAbilities->LoadAbilitiesLayout(SaveSlot);
	FirstTimeOpenInstance = false;
	TriggerSwitchOnTheFirstOpen();
}

void UAbilitiesMenuUI::CloseUI()
{
	if (bIsAbilityUnlockPopupOpened)
	{
		FadeOutUnlockAbilityPopup();
		return;
	}
	Toggle(false);
	const TObjectPtr<USaveSlot> SaveSlot = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot();
	GuardianCharacterAbilities->SaveAbilitiesLayout(SaveSlot);
	RangerCharacterAbilities->SaveAbilitiesLayout(SaveSlot);
	TricksterCharacterAbilities->SaveAbilitiesLayout(SaveSlot);
	SaveSlot->SaveSlot();
	(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->RequestAddAbilitiesToUpgradePool.ExecuteIfBound(SaveSlot);
	(void)GetGameInstance()->GetSubsystem<UDelegateManager>()->OnAbilitiesMenuSelectionEnd.ExecuteIfBound();
}

void UAbilitiesMenuUI::NativeConstruct()
{
	Super::NativeConstruct();
	SetupUnlockAbilityPopup();
	SetupButtons();
	FadeOutAnimEndDelegate.BindDynamic(this, &UAbilitiesMenuUI::FadeOutAnimFinished);
	GuardianCharacterAbilities->SetAbilitiesIconData();
	RangerCharacterAbilities->SetAbilitiesIconData();
	TricksterCharacterAbilities->SetAbilitiesIconData();
	GuardianCharacterIcon->OnVariationClicked.BindUObject(this, &UAbilitiesMenuUI::SwitchCharacterAbilities);
	RangerCharacterIcon->OnVariationClicked.BindUObject(this, &UAbilitiesMenuUI::SwitchCharacterAbilities);
	TricksterCharacterIcon->OnVariationClicked.BindUObject(this, &UAbilitiesMenuUI::SwitchCharacterAbilities);
}

void UAbilitiesMenuUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (!bWaitingForSwitch) return;
	SwitchDelayAccumulator += InDeltaTime;
	if (SwitchDelayAccumulator < 0.2f) return;
	bWaitingForSwitch = false;
	SwitchDelayAccumulator = 0.f;
	SwitchAbilityIcons();
}

void UAbilitiesMenuUI::Toggle(const bool bState)
{
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UPlayerHUD>(EWidget::PlayerHUD)->PlayFadeOutAnim(bState);
	UGameplayStatics::SetGamePaused(GetWorld(), bState);
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(bState ? ESound::PanelFadeIn : ESound::PanelFadeOut);
	const TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->bShowMouseCursor = bState;
	GetWorld()->GetSubsystem<UWidgetManager>()->SetCurrentStateOfUI(bState ? EWidget::AbilitiesMenuUI : EWidget::None);
	if (bState)
	{
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		PlayerController->SetInputMode(FInputModeGameAndUI());
		PlayAnimationForward(FadeInAnim);
	}
	else
	{
		PlayerController->SetInputMode(FInputModeGameOnly());
		BindToAnimationFinished(FadeInAnim, FadeOutAnimEndDelegate);
		PlayAnimationReverse(FadeInAnim);
	}
}

void UAbilitiesMenuUI::SwitchCharacterAbilities(const ECharacterType InCharacterName)
{
	PlayAnimationForward(CharacterTransitionAnim);
	if (CurrentlySelectedCharacter.GetIcon()) CurrentlySelectedCharacter.GetIcon()->SetEventsEnabled(true);
	switch (InCharacterName)
	{
		case ECharacterType::Guardian:
			CurrentlySelectedCharacter.SetCurrentlySelectedCharacter(ECharacterType::Guardian, GuardianCharacterIcon);
			ActiveWidgetIndex = 0;
			break;
		case ECharacterType::Ranger:
			CurrentlySelectedCharacter.SetCurrentlySelectedCharacter(ECharacterType::Ranger, RangerCharacterIcon);
			ActiveWidgetIndex = 1;
			break;
		case ECharacterType::Trickster:
			CurrentlySelectedCharacter.SetCurrentlySelectedCharacter(ECharacterType::Trickster, TricksterCharacterIcon);
			ActiveWidgetIndex = 2;
		default: ;
	}
	bWaitingForSwitch = true;
}

void UAbilitiesMenuUI::TriggerSwitchOnTheFirstOpen()
{
	SwitchCharacterAbilities(ECharacterType::Guardian);
	GuardianCharacterIcon->PlayHoverAnim();
	GuardianCharacterIcon->SetEventsEnabled(false);
}

void UAbilitiesMenuUI::SetupButtons()
{
	FirstAbilityResetButton->GetButton()->OnClicked.AddDynamic(this, &UAbilitiesMenuUI::FirstSlotResetClicked);
	SecondAbilityResetButton->GetButton()->OnClicked.AddDynamic(this, &UAbilitiesMenuUI::SecondSlotResetClicked);
	CloseButton->Get()->OnClicked.AddDynamic(this, &UAbilitiesMenuUI::CloseUI);
}

void UAbilitiesMenuUI::OpenUnlockAbilityPopup(const int32 InAbilityIndex)
{
	bIsAbilityUnlockPopupOpened = true;
	AbilityToUnlock = InAbilityIndex;
	UnlockAbilityPopup->PlayPopupFadeInAnim(true);
}

void UAbilitiesMenuUI::SetupUnlockAbilityPopup()
{
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnOpenUnlockAbilityPopup.BindUObject(this, &UAbilitiesMenuUI::OpenUnlockAbilityPopup);
	UnlockAbilityPopup->GetAcceptButton()->Get()->OnClicked.AddDynamic(this, &UAbilitiesMenuUI::UnlockAbility);
	UnlockAbilityPopup->GetDeclineButton()->Get()->OnClicked.AddDynamic(this, &UAbilitiesMenuUI::FadeOutUnlockAbilityPopup);
}

void UAbilitiesMenuUI::UpdateSkillPointsText() const
{
	const int32 SkillPoints = GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetSkillPoints();
	SkillPointsText->SetText(FText::Format(NSLOCTEXT("ST_Localization", "SkillPoints", "Skill Points: {0}"), SkillPoints));
}

void UAbilitiesMenuUI::UpdateAbilityIcon() const
{
	switch (CurrentlySelectedCharacter.GetCharacterType())
	{
		case ECharacterType::Guardian: GuardianCharacterAbilities->SetAbilitiesIconData(); break;
		case ECharacterType::Ranger: RangerCharacterAbilities->SetAbilitiesIconData(); break;
		case ECharacterType::Trickster: TricksterCharacterAbilities->SetAbilitiesIconData(); 
		default: ;
	}
}

void UAbilitiesMenuUI::SwitchAbilityIcons() const { CharacterAbilitiesSwitcher->SetActiveWidgetIndex(ActiveWidgetIndex); }

void UAbilitiesMenuUI::FadeOutAnimFinished()
{
	UnbindAllFromAnimationFinished(FadeInAnim);
	SetVisibility(ESlateVisibility::Collapsed);
}

void UAbilitiesMenuUI::FirstSlotResetClicked()
{
	switch (CurrentlySelectedCharacter.GetCharacterType())
	{
		case ECharacterType::Guardian: GuardianCharacterAbilities->ResetFirstAbilitySlot(); break;
		case ECharacterType::Ranger: RangerCharacterAbilities->ResetFirstAbilitySlot(); break;
		case ECharacterType::Trickster: TricksterCharacterAbilities->ResetFirstAbilitySlot();
		default: ;
	}
}

void UAbilitiesMenuUI::SecondSlotResetClicked()
{
	switch (CurrentlySelectedCharacter.GetCharacterType())
	{
		case ECharacterType::Guardian: GuardianCharacterAbilities->ResetSecondAbilitySlot(); break;
		case ECharacterType::Ranger: RangerCharacterAbilities->ResetSecondAbilitySlot(); break;
		case ECharacterType::Trickster: TricksterCharacterAbilities->ResetSecondAbilitySlot();
		default: ;
	}
}

void UAbilitiesMenuUI::UnlockAbility()
{
	if (GetGameInstance()->GetSubsystem<USaveSlotManager>()->GetSaveSlot()->UnlockAbility(CurrentlySelectedCharacter.GetCharacterType(), AbilityToUnlock))
	{
		UpdateSkillPointsText();
		UpdateAbilityIcon();
	}
	FadeOutUnlockAbilityPopup();
}

void UAbilitiesMenuUI::FadeOutUnlockAbilityPopup()
{
	UnlockAbilityPopup->PlayPopupFadeInAnim(false);
	bIsAbilityUnlockPopupOpened = false;
}
