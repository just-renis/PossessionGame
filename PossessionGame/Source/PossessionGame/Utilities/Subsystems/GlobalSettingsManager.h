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
	bool GetIsDemo() const { return bIsDemo; }
	FText GetKeyBindText(const FName& InKeyName) const;
	FText* GetKeyDisplayName(const FKey& InKeyName);

	void ToggleGodModeEnabled() { bIsGodModeEnabled = !bIsGodModeEnabled; }
private:
	bool bIsGodModeEnabled = false;
	bool bIsDemo = true;
	UPROPERTY()
	TMap<FKey, FText> CustomKeyDisplayNames = {
		{ EKeys::LeftMouseButton, FText::FromString("M1") },
		{ EKeys::RightMouseButton, FText::FromString("M2") }
	};
};