#include "AbilitiesMenuUI.h"
#include "PlayerHUD.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/UI/SecondaryWidgets/AbilityIconUI.h"
#include "PossessionGame/UI/SecondaryWidgets/AbilityResetButtonUI.h"
#include "PossessionGame/UI/SecondaryWidgets/CustomTextUI.h"
#include "PossessionGame/UI/SecondaryWidgets/MainMenuButtonUI.h"
#include "PossessionGame/UI/SecondaryWidgets/VariationCharacterIconUI.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void UAbilitiesMenuUI::ToggleDetailsPanel(const bool bState, const FText& InText) const
{
	if (!DetailsPanel) return;
	DetailsPanel->SetText(InText);
	DetailsPanel->PlayFadeInAnim(bState);
}

void UAbilitiesMenuUI::OpenUI()
{
	Toggle(true);
}

void UAbilitiesMenuUI::CloseUI()
{
	Toggle(false);
	/*for (const TPair<ECharacterType, FAbilitiesLayout> Layout : CharacterAbilitiesLayouts)
	{
		
	}*/
}

void UAbilitiesMenuUI::NativeConstruct()
{
	Super::NativeConstruct();
	Abilities = { FirstAbility, SecondAbility, ThirdAbility, FourthAbility };
	SetupButtons();
	TriggerSwitchOnTheFirstOpen();
	FadeOutAnimEndDelegate.BindDynamic(this, &UAbilitiesMenuUI::FadeOutAnimFinished);
	GuardianCharacterIcon->OnVariationClicked.BindUObject(this, &UAbilitiesMenuUI::SwitchCharacterAbilities);
	RangerCharacterIcon->OnVariationClicked.BindUObject(this, &UAbilitiesMenuUI::SwitchCharacterAbilities);
	TricksterCharacterIcon->OnVariationClicked.BindUObject(this, &UAbilitiesMenuUI::SwitchCharacterAbilities);
}

void UAbilitiesMenuUI::Toggle(const bool bState)
{
	GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UPlayerHUD>(EWidget::PlayerHUD, 0)->PlayFadeOutAnim(bState);
	UGameplayStatics::SetGamePaused(GetWorld(), bState);
	GetGameInstance()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(bState ? ESound::PanelFadeIn : ESound::PanelFadeOut);
	const TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->bShowMouseCursor = bState;
	GetWorld()->GetSubsystem<UWidgetManager>()->SetCurrentStateOfUI(bState ? EWidget::AbilitiesMenuUI : EWidget::None);
	if (ConfirmButton) ConfirmButton->SetButtonVisibility(bState);
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
	SaveTempAbilities();
	if (CurrentlySelectedCharacter.GetIcon()) CurrentlySelectedCharacter.GetIcon()->SetEventsEnabled(true);
	if (const FAbilitiesContainer* Container = AbilitiesData.Find(InCharacterName))
	{
		for (int32 i = 0; i < Abilities.Num(); i++)
		{
			const FAbilitiesContainerData Data = Container->GetData(i);
			if (Abilities[i]) Abilities[i]->SetAbilityData(Data.Icon ? Data.Icon : LockedTexture, Data.Details);
		}
	}
	switch (InCharacterName)
	{
		case ECharacterType::Guardian: CurrentlySelectedCharacter.SetCurrentlySelectedCharacter(ECharacterType::Guardian, GuardianCharacterIcon); break;
		case ECharacterType::Ranger: CurrentlySelectedCharacter.SetCurrentlySelectedCharacter(ECharacterType::Ranger, RangerCharacterIcon); break;
		case ECharacterType::Trickster: CurrentlySelectedCharacter.SetCurrentlySelectedCharacter(ECharacterType::Trickster, TricksterCharacterIcon);
		default: ;
	}
	LoadAbilitiesLayout();
}

void UAbilitiesMenuUI::TriggerSwitchOnTheFirstOpen()
{
	SwitchCharacterAbilities(ECharacterType::Guardian);
	GuardianCharacterIcon->PlayHoverAnim();
	GuardianCharacterIcon->SetEventsEnabled(false);
}

void UAbilitiesMenuUI::SetupButtons()
{
	FirstAbility->GetButton()->OnClicked.AddDynamic(this, &UAbilitiesMenuUI::FirstAbilitySelected);
	SecondAbility->GetButton()->OnClicked.AddDynamic(this, &UAbilitiesMenuUI::SecondAbilitySelected);
	FirstAbilityResetButton->GetButton()->OnClicked.AddDynamic(this, &UAbilitiesMenuUI::FirstSlotResetClicked);
	SecondAbilityResetButton->GetButton()->OnClicked.AddDynamic(this, &UAbilitiesMenuUI::SecondSlotResetClicked);
}

void UAbilitiesMenuUI::SaveTempAbilities()
{
	if (CurrentlySelectedCharacter.GetCharacterType() == ECharacterType::Random) return;
	FAbilitiesLayout* Layout = &CharacterAbilitiesLayouts.FindOrAdd(CurrentlySelectedCharacter.GetCharacterType());
	if (FirstAbilitySlot) SaveAbilitySlot(FirstAbilitySlot, Layout, Select1To1Anim, Select2To1Anim, true);
	if (SecondAbilitySlot) SaveAbilitySlot(SecondAbilitySlot, Layout, Select1To2Anim, Select2To2Anim, false);
}

void UAbilitiesMenuUI::LoadAbilitiesLayout()
{
	const FAbilitiesLayout* Layout = CharacterAbilitiesLayouts.Find(CurrentlySelectedCharacter.GetCharacterType());
	if (!Layout) return;
	const int32 FirstAbilityIndex = Abilities.IndexOfByKey(FirstAbility);
	const int32 SecondAbilityIndex = Abilities.IndexOfByKey(SecondAbility);
	LoadAbilityToSlot(FirstAbilitySlot, Layout, FirstAbilityIndex, SecondAbilityIndex, Select1To1Anim, Select2To1Anim, true);
	LoadAbilityToSlot(SecondAbilitySlot, Layout, FirstAbilityIndex, SecondAbilityIndex, Select1To2Anim, Select2To2Anim, false);
}

void UAbilitiesMenuUI::SaveAbilitySlot(TObjectPtr<UAbilityIconUI>& InAbilitySlot, FAbilitiesLayout* InLayout, const TObjectPtr<UWidgetAnimation>& InFirstAnim, const TObjectPtr<UWidgetAnimation>& InSecondAnim, const bool bIsFirstSlot)
{
	if (InAbilitySlot == FirstAbility)
	{
		PlayAnimation(InFirstAnim);
		StopAnimation(InFirstAnim);
		FirstAbility->SetIsInSlot(false);
		if (bIsFirstSlot) InLayout->FirstAbilityIndex = 0;
		else InLayout->SecondAbilityIndex = 0;
	}
	else if (InAbilitySlot == SecondAbility)
	{
		PlayAnimation(InSecondAnim);
		StopAnimation(InSecondAnim);
		SecondAbility->SetIsInSlot(false);
		if (bIsFirstSlot) InLayout->FirstAbilityIndex = 1;
		else InLayout->SecondAbilityIndex = 1;
	}
	InAbilitySlot = nullptr;
}

void UAbilitiesMenuUI::LoadAbilityToSlot(TObjectPtr<UAbilityIconUI>& InAbilitySlot, const FAbilitiesLayout* InLayout, const int32 InFirstAbilityIndex, const int32 InSecondAbilityIndex, const TObjectPtr<UWidgetAnimation>& InFirstAnim, const TObjectPtr<UWidgetAnimation>& InSecondAnim, const bool bIsFirstSlot)
{
	const int32 LayoutIndex = bIsFirstSlot ? InLayout->FirstAbilityIndex : InLayout->SecondAbilityIndex;
	if (LayoutIndex == InFirstAbilityIndex)
	{
		PlayAnimation(InFirstAnim);
		FirstAbility->SetIsInSlot(true);
		InAbilitySlot = FirstAbility;
	}
	else if (LayoutIndex == InSecondAbilityIndex)
	{
		PlayAnimation(InSecondAnim);
		SecondAbility->SetIsInSlot(true);
		InAbilitySlot = SecondAbility;
	}
}

void UAbilitiesMenuUI::FadeOutAnimFinished()
{
	UnbindAllFromAnimationFinished(FadeInAnim);
	SetVisibility(ESlateVisibility::Collapsed);
}

void UAbilitiesMenuUI::FirstAbilitySelected()
{
	if (FirstAbility->GetIsInSlot()) return;
	FirstAbility->SetIsInSlot(true);
	if (!FirstAbilitySlot)
	{
		FirstAbilitySlot = FirstAbility;
		PlayAnimationForward(Select1To1Anim);
	}
	else if (!SecondAbilitySlot)
	{
		SecondAbilitySlot = FirstAbility;
		PlayAnimationForward(Select1To2Anim);
	}
	else FirstAbility->PlayWobbleAnim();
}

void UAbilitiesMenuUI::SecondAbilitySelected()
{
	if (SecondAbility->GetIsInSlot()) return;
	SecondAbility->SetIsInSlot(true);
	if (!FirstAbilitySlot)
	{
		FirstAbilitySlot = SecondAbility;
		PlayAnimationForward(Select2To1Anim);
	}
	else if (!SecondAbilitySlot)
	{
		SecondAbilitySlot = SecondAbility;
		PlayAnimationForward(Select2To2Anim);
	}
	else SecondAbility->PlayWobbleAnim();
}

void UAbilitiesMenuUI::FirstSlotResetClicked()
{
	if (!FirstAbilitySlot) return;
	if (FirstAbilitySlot == FirstAbility) PlayAnimationReverse(Select1To1Anim);
	else if (FirstAbilitySlot == SecondAbility) PlayAnimationReverse(Select2To1Anim);
	FirstAbilitySlot->SetIsInSlot(false);
	FirstAbilitySlot = nullptr;
	if (FAbilitiesLayout* Layout = CharacterAbilitiesLayouts.Find(CurrentlySelectedCharacter.GetCharacterType())) Layout->FirstAbilityIndex = INDEX_NONE;
}

void UAbilitiesMenuUI::SecondSlotResetClicked()
{
	if (!SecondAbilitySlot) return;
	if (SecondAbilitySlot == FirstAbility) PlayAnimationReverse(Select1To2Anim);
	else if (SecondAbilitySlot == SecondAbility) PlayAnimationReverse(Select2To2Anim);
	SecondAbilitySlot->SetIsInSlot(false);
	SecondAbilitySlot = nullptr;
	if (FAbilitiesLayout* Layout = CharacterAbilitiesLayouts.Find(CurrentlySelectedCharacter.GetCharacterType())) Layout->SecondAbilityIndex = INDEX_NONE;
}
