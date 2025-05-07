#include "ControlsSettingsUI.h"
#include "EnhancedInputSubsystems.h"
#include "KeyBindingUI.h"
#include "Components/VerticalBox.h"
#include "UserSettings/EnhancedInputUserSettings.h"

bool UControlsSettingsUI::IsKeyBindsHasDuplicates()
{
	TSet<FKey> Keys;
	for (int32 i = 0; i < KeyBindings.Num(); i++) Keys.Add(KeyBindings[i]->GetKeyBindKey());
	return Keys.Num() != KeyBindings.Num();
}

void UControlsSettingsUI::NativeConstruct()
{
	Super::NativeConstruct();
	UserSettings = GetWorld()->GetFirstPlayerController()->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>()->GetUserSettings();
	if (!UserSettings) return;
	if (!UserSettings->IsMappingContextRegistered(MappingContext)) UserSettings->RegisterInputMappingContext(MappingContext);
	SetupKeyBinds();
}

void UControlsSettingsUI::SetupKeyBinds()
{
	TArray DesiredOrder = {
		FName("MoveForward"),
		FName("MoveBackward"),
		FName("MoveLeft"),
		FName("MoveRight"),
		FName("Possess"),
		FName("Dash"),
		FName("Jump"),
		FName("Crouch"),
		FName("Reload"),
		FName("LeftClick"),
		FName("RightClick"),
		FName("FirstAbility"),
		FName("SecondAbility"),
		FName("ThirdAbility"),
		FName("Escape"),
		FName("Interact"),
		FName("QuickMelee")
	};
	TMap<FName, FPlayerKeyMapping> OrderedKeyBinds;
	SetupOrderOfKeyBinds(DesiredOrder, OrderedKeyBinds);
	AddKeyBinds(DesiredOrder, OrderedKeyBinds);
}

void UControlsSettingsUI::SetupOrderOfKeyBinds(TArray<FName>& DesiredOrder, TMap<FName, FPlayerKeyMapping>& OrderedKeyBinds)
{
	for (FName ActionName : DesiredOrder)
	{
		for (FPlayerKeyMapping KeyBind : UserSettings->GetCurrentKeyProfile()->GetPlayerMappingRows().Find(ActionName)->Mappings)
		{
			OrderedKeyBinds.Add(ActionName, KeyBind);
			break;
		}
	}
}

void UControlsSettingsUI::AddKeyBinds(TArray<FName>& DesiredOrder, TMap<FName, FPlayerKeyMapping>& OrderedKeyBinds)
{
	for (FName ActionName : DesiredOrder)
	{
		TObjectPtr<UKeyBindingUI> KeyBindingWidget = CreateWidget<UKeyBindingUI>(this, KeyBindingClass);
		KeyBindingWidget->Setup(UserSettings, *OrderedKeyBinds.Find(ActionName));
		KeyBindings.Add(KeyBindingWidget);
		if (KeyBindingsPanel) KeyBindingsPanel->AddChild(KeyBindingWidget);
	}
}

void UControlsSettingsUI::ResetKeyBindsToDefault()
{
	for (int32 i = 0; i < KeyBindings.Num(); i++) if (KeyBindings[i]) KeyBindings[i]->ResetKeyBind();
}

bool UControlsSettingsUI::ApplySettings()
{
	if (!IsKeyBindsHasDuplicates())
	{
		for (int32 i = 0; i < KeyBindings.Num(); i++) if (KeyBindings[i]) KeyBindings[i]->RemapKeyBind();
		UserSettings->ApplySettings();
		UserSettings->SaveSettings();
		return true;
	}
	return false;
}

void UControlsSettingsUI::SetSavedSettings()
{
	for (int32 i = 0; i < KeyBindings.Num(); i++) if (KeyBindings[i]) KeyBindings[i]->SetSavedKeyBind();
}
