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
	bool GetIsCutscenePlaying() const { return bIsCutscenePlaying; }
	bool GetIsDemo() const { return bIsDemo; }

	void ToggleGodModeEnabled() { bIsGodModeEnabled = !bIsGodModeEnabled; }
	void Init(const TObjectPtr<UDelegateManager>& InDelegateManager);
private:
	bool bIsGodModeEnabled = false;
	bool bIsDemo = true;
	bool bIsCutscenePlaying = false;

	void SetCutsceneState(const bool bState) { bIsCutscenePlaying = bState; }
};