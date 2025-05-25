#include "KeyBindingUI.h"
#include "SettingsUI.h"
#include "Components/Button.h"
#include "Components/InputKeySelector.h"
#include "Components/TextBlock.h"
#include "PossessionGame/UI/SecondaryWidgets/SettingsTitleUI.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/GlobalSettingsManager.h"

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
	if (ResetKeyBindingButton) ResetKeyBindingButton->OnClicked.AddDynamic(this, &UKeyBindingUI::ResetKeyBind);
	if (InputKeySelector) InputKeySelector->OnClicked.AddDynamic(this, &UKeyBindingUI::OnSelectingKeyChanged);
///	if (InputKeySelector) InputKeySelector->OnIsSelectingKeyChanged.AddDynamic(this, &UKeyBindingUI::OnSelectingKeyChanged);
}

void UKeyBindingUI::OnSelectingKeyChanged()
{
	InputKeySelector->SetIsEnabled(false);
	UE_LOG(LogTemp, Error, TEXT("BEGIN REBIND"));
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
	if (!InputKeySelector || !KeyBindText) return;
	//InputKeySelector->SetSelectedKey(Mapping.GetCurrentKey());
	const FText* CustomName = GetGameInstance()->GetSubsystem<UGlobalSettingsManager>()->GetKeyDisplayName(Mapping.GetCurrentKey());
	const FText DisplayName = CustomName ? *CustomName : Mapping.GetCurrentKey().GetDisplayName();
	KeyBindText->SetText(DisplayName);
	InputKeySelector->InvalidateLayoutAndVolatility();
}

void UKeyBindingUI::CancelRebind() const { if (InputKeySelector) InputKeySelector->SetIsEnabled(true); }

void UKeyBindingUI::OnKeySelected(FInputChord InMapping)
{
	TempSelectedKey = InMapping.Key;
	InputKeySelector->SetIsEnabled(true);
	if (KeyBindText) KeyBindText->SetText(GetDisplayName(TempSelectedKey));
}

void UKeyBindingUI::ResetKeyBind()
{
	if (!InputKeySelector || !KeyBindText) return;
	//InputKeySelector->SetSelectedKey(Mapping.GetDefaultKey());
	KeyBindText->SetText(GetDisplayName(Mapping.GetDefaultKey()));
	InputKeySelector->InvalidateLayoutAndVolatility();
}
