#include "KeyBindingUI.h"
#include "Components/Button.h"
#include "Components/InputKeySelector.h"
#include "PossessionGame/UI/SecondaryWidgets/SettingsTitleUI.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"

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
	int32 Index = -1;
	const FName MappingName = Mapping.GetMappingName();
	if (MappingName == "FirstAbility") Index = 0;
	else if (MappingName == "SecondAbility") Index = 1;
	else if (MappingName == "ThirdAbility") Index = 2;
	if (Index != -1) (void)GetGameInstance()->GetSubsystem<UDelegateManager>()->OnKeyBindChange.ExecuteIfBound(Index, Args.NewKey.GetDisplayName());
	FGameplayTagContainer FailureReason;
	UserSettings->MapPlayerKey(Args, FailureReason);
	Mapping.SetCurrentKey(TempSelectedKey);
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
