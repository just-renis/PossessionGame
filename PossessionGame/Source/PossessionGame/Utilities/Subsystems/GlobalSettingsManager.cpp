#include "GlobalSettingsManager.h"

#include "DelegateManager.h"
#include "EnhancedInputSubsystems.h"
#include "UserSettings/EnhancedInputUserSettings.h"

FText UGlobalSettingsManager::GetKeyBindText(const FName& InKeyName) const
{
	const TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController<APlayerController>();
	if (!PlayerController) return FText::GetEmpty();
	const TObjectPtr<UEnhancedInputLocalPlayerSubsystem> Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	if (!Subsystem) return FText::GetEmpty();
	const TObjectPtr<UEnhancedInputUserSettings> UserSettings = Subsystem->GetUserSettings();
	if (!UserSettings) return FText::GetEmpty();
	const TObjectPtr<UEnhancedPlayerMappableKeyProfile> CurrentProfile = UserSettings->GetActiveKeyProfile();
	if (!CurrentProfile) return FText::GetEmpty();
	const FKeyMappingRow* MappingRow = CurrentProfile->GetPlayerMappingRows().Find(InKeyName);
	if (!MappingRow || MappingRow->Mappings.Num() == 0) return FText::GetEmpty();
	for (const FPlayerKeyMapping KeyBind : MappingRow->Mappings) return KeyBind.GetCurrentKey().GetDisplayName();
	return FText::GetEmpty();
}

FText* UGlobalSettingsManager::GetKeyDisplayName(const FKey& InKeyName) { return CustomKeyDisplayNames.Find(InKeyName); }

FText* UGlobalSettingsManager::GetLocalizedMappingName(const FString& InMappingName) { return LocalizedActionDisplayNames.Find(InMappingName); }

void UGlobalSettingsManager::Init(const TObjectPtr<UDelegateManager>& InDelegateManager) { InDelegateManager->OnXrayChange.AddUObject(this, &UGlobalSettingsManager::SetXray); }
