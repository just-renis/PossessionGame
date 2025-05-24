#include "KeyBindingUI.h"
#include "Components/Button.h"
#include "Components/InputKeySelector.h"
#include "PossessionGame/UI/SecondaryWidgets/SettingsTitleUI.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/GlobalSettingsManager.h"

void UKeyBindingUI::Setup(const TObjectPtr<UEnhancedInputUserSettings>& InUserSettings, const FPlayerKeyMapping& InMapping)
{
	UserSettings = InUserSettings;
	Mapping = InMapping;
	TempSelectedKey = Mapping.GetCurrentKey();
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
	if (ResetKeyBindingButton) ResetKeyBindingButton->OnClicked.AddDynamic(this, &UKeyBindingUI::ResetKeyBind);
	if (InputKeySelector) InputKeySelector->OnKeySelected.AddDynamic(this, &UKeyBindingUI::OnKeySelected);
}

void UKeyBindingUI::OnKeySelected(FInputChord InMapping) { TempSelectedKey = InMapping.Key; }

void UKeyBindingUI::RemapKeyBind()
{
	FMapPlayerKeyArgs Args;
	Args.MappingName = Mapping.GetMappingName();
	Args.Slot = EPlayerMappableKeySlot::First;
	Args.NewKey = TempSelectedKey;
	GetGameInstance()->GetSubsystem<UDelegateManager>()->OnKeyBindChange.Broadcast(Mapping.GetMappingName(), Args.NewKey.GetDisplayName());
	FGameplayTagContainer FailureReason;
	UserSettings->MapPlayerKey(Args, FailureReason);
	Mapping.SetCurrentKey(TempSelectedKey);
	//const FText* CustomName = GetGameInstance()->GetSubsystem<UGlobalSettingsManager>()->GetKeyDisplayName(Mapping.GetCurrentKey());
	///const FText DisplayName = CustomName ? *CustomName : Mapping.GetCurrentKey().GetDisplayName();
//	InputKeySelector->SetKeySelectionText(DisplayName);
}

void UKeyBindingUI::SetSavedKeyBind() const
{
	if (InputKeySelector)
	{
		InputKeySelector->SetSelectedKey(Mapping.GetCurrentKey());
		InputKeySelector->InvalidateLayoutAndVolatility();
	}
}

void UKeyBindingUI::ResetKeyBind()
{
	if (InputKeySelector)
	{
		InputKeySelector->SetSelectedKey(Mapping.GetDefaultKey());
		InputKeySelector->InvalidateLayoutAndVolatility();
	}
}
