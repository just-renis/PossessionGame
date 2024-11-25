#pragma once

#include "CoreMinimal.h"
#include "GlobalSettingsManager.generated.h"

UCLASS()
class POSSESSIONGAME_API UGlobalSettingsManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	bool GetIsGodModeEnabled() { return bIsGodModeEnabled; }

	void ToggleGodModeEnabled() { bIsGodModeEnabled = !bIsGodModeEnabled; }
private:
	bool bIsGodModeEnabled = false;
};