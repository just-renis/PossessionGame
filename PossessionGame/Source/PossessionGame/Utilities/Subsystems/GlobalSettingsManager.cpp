#include "GlobalSettingsManager.h"
#include "DelegateManager.h"

void UGlobalSettingsManager::Init(const TObjectPtr<UDelegateManager>& InDelegateManager)
{
	InDelegateManager->OnCutsceneStateChange.AddUObject(this, &UGlobalSettingsManager::SetCutsceneState);
}
