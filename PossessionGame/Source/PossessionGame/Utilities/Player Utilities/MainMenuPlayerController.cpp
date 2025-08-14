#include "MainMenuPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "PossessionGame/UI/MainWidgets/MainMenuUI.h"
#include "PossessionGame/UI/MainWidgets/SaveSlotMenuUI.h"
#include "PossessionGame/UI/MainWidgets/SettingsUI.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"
#include "UserSettings/EnhancedInputUserSettings.h"

void AMainMenuPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	SetupUserSettings();
	if (const TObjectPtr<UEnhancedInputLocalPlayerSubsystem> Subsystem = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>()) Subsystem->AddMappingContext(PlayerMappingContext, 0);
	if (const TObjectPtr<UEnhancedInputComponent> EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(PlayerMappingContext->GetMapping(15).Action, ETriggerEvent::Started, this, &AMainMenuPlayerController::HandleEscapeClick);
	}
}

void AMainMenuPlayerController::HandleEscapeClick()
{
	switch (GetWorld()->GetSubsystem<UWidgetManager>()->GetCurrentStateOfUI())
	{
		case EWidget::MainMenuUI: GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<UMainMenuUI>(EWidget::MainMenuUI)->CloseUI(); break;
		case EWidget::SettingsUI: GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<USettingsUI>(EWidget::SettingsUI)->CloseUI(); break;
		case EWidget::SaveSlotMenuUI: GetWorld()->GetSubsystem<UWidgetManager>()->GetWidget<USaveSlotMenuUI>(EWidget::SaveSlotMenuUI)->CloseUI();
		default: ;
	}
}

void AMainMenuPlayerController::SetupUserSettings() const
{
	const TObjectPtr<UEnhancedInputUserSettings> UserSettings = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>()->GetUserSettings();
	if (!UserSettings) return;
	if (!UserSettings->IsMappingContextRegistered(PlayerMappingContext)) UserSettings->RegisterInputMappingContext(PlayerMappingContext);
}
