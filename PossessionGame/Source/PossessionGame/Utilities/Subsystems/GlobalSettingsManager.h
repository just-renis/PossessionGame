#pragma once

#include "CoreMinimal.h"
#include "GlobalSettingsManager.generated.h"

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

	void ToggleGodModeEnabled() { bIsGodModeEnabled = !bIsGodModeEnabled; }
	void SetMouseHorizontalSensitivity(const float InValue) { MouseSensitivity.X = InValue; }
	void SetMouseVerticalSensitivity(const float InValue) { MouseSensitivity.Y = InValue; }
private:
	bool bIsGodModeEnabled = false;
	FVector2D MouseSensitivity = FVector2D(1.f, 1.f);
	UPROPERTY()
	TMap<FKey, FText> CustomKeyDisplayNames = {
		{ EKeys::LeftMouseButton, FText::FromString("M1") },
		{ EKeys::RightMouseButton, FText::FromString("M2") },
		{ EKeys::LeftShift, FText::FromString("L Shift") },
		{ EKeys::LeftControl, FText::FromString("L Ctrl") },
		{ EKeys::ThumbMouseButton, FText::FromString("TMB") },
		{ EKeys::ThumbMouseButton2, FText::FromString("TMB2") },
	};
};