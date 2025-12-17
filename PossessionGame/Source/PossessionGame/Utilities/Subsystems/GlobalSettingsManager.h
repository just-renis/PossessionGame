#pragma once

#include "CoreMinimal.h"
#include "GlobalSettingsManager.generated.h"

class UCodexData;
class UDelegateManager;

UCLASS()
class POSSESSIONGAME_API UGlobalSettingsManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	bool GetIsGodModeEnabled() const { return bIsGodModeEnabled; }
	FVector2D GetMouseSensitivity() const { return MouseSensitivity; }
	FText GetKeyBindText(const FName& InKeyName) const;
	FText* GetKeyDisplayName(const FKey& InKeyName);
	FText* GetLocalizedMappingName(const FString& InMappingName);
	TObjectPtr<UCodexData> GetCodexData() const { return CodexData; }

	void ToggleGodModeEnabled() { bIsGodModeEnabled = !bIsGodModeEnabled; }
	void SetMouseHorizontalSensitivity(const float InValue) { MouseSensitivity.X = InValue; }
	void SetMouseVerticalSensitivity(const float InValue) { MouseSensitivity.Y = InValue; }
	void SetCodexData(const TObjectPtr<UCodexData>& InCodexData) { CodexData = InCodexData; }
private:
	bool bIsGodModeEnabled = false;
	FVector2D MouseSensitivity = FVector2D(1.f, 1.f);
	UPROPERTY()
	TObjectPtr<UCodexData> CodexData;
	UPROPERTY()
	TMap<FKey, FText> CustomKeyDisplayNames = {
		{ EKeys::LeftMouseButton, NSLOCTEXT("ST_Localization", "M1", "M1") },
		{ EKeys::RightMouseButton, NSLOCTEXT("ST_Localization", "M2", "M2") },
		{ EKeys::LeftShift, NSLOCTEXT("ST_Localization", "L Shift", "L Shift") },
		{ EKeys::LeftControl, NSLOCTEXT("ST_Localization", "L Ctrl", "L Ctrl") },
		{ EKeys::ThumbMouseButton, NSLOCTEXT("ST_Localization", "TMB1", "TMB1") },
		{ EKeys::ThumbMouseButton2, NSLOCTEXT("ST_Localization", "TMB2", "TMB2") },
	};
	UPROPERTY()
	TMap<FString, FText> LocalizedActionDisplayNames = {
		{ FString("Slide / Stomp"), NSLOCTEXT("ST_Localization", "Slide / Stomp", "Slide / Stomp") },
		{ FString("Back"), NSLOCTEXT("ST_Localization", "Escape", "Escape") },
		{ FString("First Ability"), NSLOCTEXT("ST_Localization", "First Ability", "First Ability") },
		{ FString("Hijack"), NSLOCTEXT("ST_Localization", "Hijack", "Hijack") },
		{ FString("Interact"), NSLOCTEXT("ST_Localization", "Interact", "Interact") },
		{ FString("Jump"), NSLOCTEXT("ST_Localization", "Jump", "Jump") },
		{ FString("Shoot"), NSLOCTEXT("ST_Localization", "Shoot", "Shoot") },
		{ FString("Melee"), NSLOCTEXT("ST_Localization", "Melee", "Melee") },
		{ FString("Forward"), NSLOCTEXT("ST_Localization", "Forward", "Forward") },
		{ FString("Backward"), NSLOCTEXT("ST_Localization", "Backward", "Backward") },
		{ FString("Left"), NSLOCTEXT("ST_Localization", "Left", "Left") },
		{ FString("Right"), NSLOCTEXT("ST_Localization", "Right", "Right") },
		{ FString("Reload"), NSLOCTEXT("ST_Localization", "Reload", "Reload") },
		{ FString("Run"), NSLOCTEXT("ST_Localization", "Run", "Run") },
		{ FString("Second Ability"), NSLOCTEXT("ST_Localization", "Second Ability", "Second Ability") },
		{ FString("Open Upgrade"), NSLOCTEXT("ST_Localization", "Open Upgrade", "Open Upgrade") },
		{ FString("Info Panel"), NSLOCTEXT("ST_Localization", "Info Panel", "Info Panel") },
	};
};