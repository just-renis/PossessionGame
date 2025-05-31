#include "KeyBindingUI.h"
#include "Components/Button.h"
#include "Components/InputKeySelector.h"
#include "Components/TextBlock.h"
#include "PossessionGame/UI/MainWidgets/SettingsUI.h"
#include "PossessionGame/UI/SecondaryWidgets/SettingsTitleUI.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/GlobalSettingsManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

void UKeyBindingUI::Setup(const TObjectPtr<UEnhancedInputUserSettings>& InUserSettings, const FPlayerKeyMapping& InMapping, const TObjectPtr<USettingsUI>& InParentWidget)
{
	UserSettings = InUserSettings;
	Mapping = InMapping;
	ParentWidget = InParentWidget;
	TempSelectedKey = Mapping.GetCurrentKey();
}

FText UKeyBindingUI::GetDisplayName(const FKey& InKeyName) const
{
	const FText* CustomName = GetGameInstance()->GetSubsystem<UGlobalSettingsManager>()->GetKeyDisplayName(InKeyName);
	return CustomName ? *CustomName :InKeyName.GetDisplayName();
}

void UKeyBindingUI::NativeConstruct()
{
	Super::NativeConstruct();
	SetupEvents();
	if (Title) Title->SetTitleText(Mapping.GetDisplayName());
	SetSavedKeyBind();
}

void UKeyBindingUI::SetupEvents()
{
	if (ResetKeyBindingButton)
	{
		ResetKeyBindingButton->OnClicked.AddDynamic(this, &UKeyBindingUI::ResetSingularKeyBind);
		ResetKeyBindingButton->OnHovered.AddDynamic(this, &UKeyBindingUI::PlayResetHoverAnim);
		ResetKeyBindingButton->OnUnhovered.AddDynamic(this, &UKeyBindingUI::PlayResetUnHoverAnim);
	}
	if (InputKeySelectorButton)
	{
		InputKeySelectorButton->OnClicked.AddDynamic(this, &UKeyBindingUI::OnSelectingKeyChanged);
		InputKeySelectorButton->OnHovered.AddDynamic(this, &UKeyBindingUI::UKeyBindingUI::PlayKeyBindHoverAnim);
		InputKeySelectorButton->OnUnhovered.AddDynamic(this, &UKeyBindingUI::UKeyBindingUI::PlayKeyBindUnHoverAnim);
	}
}

void UKeyBindingUI::OnSelectingKeyChanged()
{
	if (!InputKeySelectorButton || !ParentWidget) return;
	PlayKeyBindButtonSelectAnim();
	GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::ButtonSelect);
	InputKeySelectorButton->SetIsEnabled(false);
	ParentWidget->BeginRebind(this);
}

void UKeyBindingUI::RemapKeyBind()
{
	FMapPlayerKeyArgs Args;
	Args.MappingName = Mapping.GetMappingName();
	Args.Slot = EPlayerMappableKeySlot::First;
	Args.NewKey = TempSelectedKey;
	FGameplayTagContainer FailureReason;
	UserSettings->MapPlayerKey(Args, FailureReason);
	Mapping.SetCurrentKey(TempSelectedKey);
	const FText DisplayName = GetDisplayName(Mapping.GetCurrentKey());
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnKeyBindChange.Broadcast(Mapping.GetMappingName(), DisplayName);
	KeyBindText->SetText(DisplayName);
}

void UKeyBindingUI::SetSavedKeyBind() const
{
	if (!InputKeySelectorButton || !KeyBindText) return;
	const FText* CustomName = GetGameInstance()->GetSubsystem<UGlobalSettingsManager>()->GetKeyDisplayName(Mapping.GetCurrentKey());
	const FText DisplayName = CustomName ? *CustomName : Mapping.GetCurrentKey().GetDisplayName();
	KeyBindText->SetText(DisplayName);
}

void UKeyBindingUI::CancelRebind() const { if (InputKeySelectorButton) InputKeySelectorButton->SetIsEnabled(true); }

void UKeyBindingUI::ResetKeyBind() const
{
	if (!InputKeySelectorButton || !KeyBindText) return;
	KeyBindText->SetText(GetDisplayName(Mapping.GetDefaultKey()));
}

void UKeyBindingUI::OnKeySelected(FInputChord InMapping)
{
	TempSelectedKey = InMapping.Key;
	InputKeySelectorButton->SetIsEnabled(true);
	if (KeyBindText) KeyBindText->SetText(GetDisplayName(TempSelectedKey));
}

void UKeyBindingUI::ResetSingularKeyBind()
{
	PlayResetButtonSelectAnim();
	ResetKeyBind();
	GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::ClickArrow);
}

void UKeyBindingUI::PlayResetButtonSelectAnim() { PlayAnimation(ResetSelectAnim); }

void UKeyBindingUI::PlayResetHoverAnim()
{
	GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::ButtonHover);
	PlayAnimation(ResetHoverAnim, GetAnimationCurrentTime(ResetHoverAnim));
}

void UKeyBindingUI::PlayResetUnHoverAnim()
{
	PlayAnimation(ResetHoverAnim, GetAnimationCurrentTime(ResetHoverAnim), 1, EUMGSequencePlayMode::Reverse);
}

void UKeyBindingUI::PlayKeyBindButtonSelectAnim() { PlayAnimation(KeyBindSelectAnim); }

void UKeyBindingUI::PlayKeyBindHoverAnim()
{
	GetWorld()->GetSubsystem<UPrimaryDataAssetsManager>()->PlaySound2D(ESound::ButtonHover);
	PlayAnimation(KeyBindHoverAnim, GetAnimationCurrentTime(KeyBindHoverAnim));
}

void UKeyBindingUI::PlayKeyBindUnHoverAnim()
{
	PlayAnimation(KeyBindHoverAnim, GetAnimationCurrentTime(KeyBindHoverAnim), 1, EUMGSequencePlayMode::Reverse);
}
