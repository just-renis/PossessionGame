#include "ControlsSettingsUI.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Image.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "PossessionGame/UI/SecondaryWidgets/KeyBindingUI.h"
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
}

void UControlsSettingsUI::SetupKeyBinds()
{
	TArray DesiredOrder = {
		FName("MoveForward"),
		FName("MoveBackward"),
		FName("MoveLeft"),
		FName("MoveRight"),
		FName("Jump"),
		FName("Run"),
		FName("Crouch"),
		FName("Shoot"),
		FName("SecondaryShoot"),
		FName("Stomp"),
		FName("Melee"),
		FName("FirstAbility"),
		FName("SecondAbility"),
		FName("Hijack"),
		FName("Reload"),
		FName("Escape"),
		FName("Interact"),
		FName("InfoPanel")
	};
	TMap<FName, FPlayerKeyMapping> OrderedKeyBinds;
	SetupOrderOfKeyBinds(DesiredOrder, OrderedKeyBinds);
	AddKeyBinds(DesiredOrder, OrderedKeyBinds);
}

void UControlsSettingsUI::SetupOrderOfKeyBinds(TArray<FName>& DesiredOrder, TMap<FName, FPlayerKeyMapping>& OrderedKeyBinds)
{
	for (FName ActionName : DesiredOrder)
	{
		for (FPlayerKeyMapping KeyBind : UserSettings->GetActiveKeyProfile()->GetPlayerMappingRows().Find(ActionName)->Mappings)
		{
			OrderedKeyBinds.Add(ActionName, KeyBind);
			break;
		}
	}
}

void UControlsSettingsUI::AddKeyBinds(TArray<FName>& DesiredOrder, TMap<FName, FPlayerKeyMapping>& OrderedKeyBinds)
{
	if (!KeyBindingsPanel) return;
	for (int32 i = 0; i < DesiredOrder.Num(); i++)
	{
		TObjectPtr<UKeyBindingUI> KeyBindingWidget = CreateWidget<UKeyBindingUI>(this, KeyBindingClass);
		KeyBindingWidget->Setup(UserSettings, *OrderedKeyBinds.Find(DesiredOrder[i]), ParentWidget);
		KeyBindings.Add(KeyBindingWidget);
		if (const TObjectPtr<UVerticalBoxSlot> ElementSlot = Cast<UVerticalBoxSlot>(KeyBindingsPanel->AddChild(KeyBindingWidget)))
		{
			FSlateChildSize ChildSize(ESlateSizeRule::Fill);
			ChildSize.Value = 1.f;
			ElementSlot->SetSize(ChildSize);
			if (i != DesiredOrder.Num() - 1 && i != DesiredOrderMovement && i != DesiredOrderAction) ElementSlot->SetPadding(FMargin(0.f, 0.f, 0.f, 50.f));
		}
		if (i != DesiredOrderMovement && i != DesiredOrderAction) continue;
		const TObjectPtr<UImage> LineSeparator = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
		if (!LineSeparator) continue;
		LineSeparator->SetBrushFromAtlasInterface(LineSeparatorTexture);
		if (const TObjectPtr<UVerticalBoxSlot> ElementSlot = Cast<UVerticalBoxSlot>(KeyBindingsPanel->AddChild(LineSeparator)))
		{
			FSlateChildSize ChildSize(ESlateSizeRule::Fill);
			ChildSize.Value = 0.1f;
			ElementSlot->SetSize(ChildSize);
			ElementSlot->SetPadding(FMargin(0.f, 50.f));
		}
	}
}

void UControlsSettingsUI::ResetKeyBindsToDefault()
{
	for (int32 i = 0; i < KeyBindings.Num(); i++) if (KeyBindings[i]) KeyBindings[i]->ResetKeyBind();
}

void UControlsSettingsUI::Setup(const TObjectPtr<USettingsUI>& InParent)
{
	ParentWidget = InParent;
	SetupKeyBinds();
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
