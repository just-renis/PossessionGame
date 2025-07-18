#include "AbilitiesMenuUI.h"
#include "PlayerHUD.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/UI/SecondaryWidgets/AbilityIconUI.h"
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
}

void UAbilitiesMenuUI::NativeConstruct()
{
	Super::NativeConstruct();
	Abilities = { FirstAbility, SecondAbility, ThirdAbility, FourthAbility };
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
	if (CurrentlySelectedIcon) CurrentlySelectedIcon->SetEventsEnabled(true);
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
		case ECharacterType::Guardian: CurrentlySelectedIcon = GuardianCharacterIcon; break;
		case ECharacterType::Ranger: CurrentlySelectedIcon = RangerCharacterIcon; break;
		case ECharacterType::Trickster: CurrentlySelectedIcon = TricksterCharacterIcon;
		default: ;
	}
}

void UAbilitiesMenuUI::TriggerSwitchOnTheFirstOpen()
{
	SwitchCharacterAbilities(ECharacterType::Guardian);
	GuardianCharacterIcon->PlayHoverAnim();
	GuardianCharacterIcon->SetEventsEnabled(false);
}

void UAbilitiesMenuUI::FadeOutAnimFinished()
{
	UnbindAllFromAnimationFinished(FadeInAnim);
	SetVisibility(ESlateVisibility::Collapsed);
}
