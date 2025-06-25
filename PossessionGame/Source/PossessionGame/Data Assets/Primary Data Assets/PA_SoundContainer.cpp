#include "PA_SoundContainer.h"

FPrimaryAssetId UPA_SoundContainer::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(TEXT("Sound"), SoundName);
}
