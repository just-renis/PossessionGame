#include "PA_SoundContainer.h"

FPrimaryAssetId UPA_SoundContainer::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(TEXT("Sound"), SoundName);
}

void UPA_SoundContainer::TriggerSoundsCallbacks()
{
	for (const FTFunctionCallback Callback : SoundsLoadedCallbacks) if (Callback.Function) Callback.Function(); 
	SoundsLoadedCallbacks.Empty();
}
