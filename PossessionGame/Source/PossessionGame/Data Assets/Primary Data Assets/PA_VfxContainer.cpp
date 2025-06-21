#include "PA_VfxContainer.h"

TMap<FName, FNiagaraUserParam>* UPA_VfxContainer::GetParams(const EVariation InVariationName)
{
	if (FVariationVfxParams* Found = VariationParams.Find(InVariationName)) return &Found->Params;
	return nullptr;
}

FPrimaryAssetId UPA_VfxContainer::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(TEXT("Vfx"), VfxPrimaryName);
}

void UPA_VfxContainer::TriggerVfxCallbacks()
{
	for (const FTFunctionCallback Callback : VfxLoadedCallbacks) if (Callback.Function) Callback.Function();
	VfxLoadedCallbacks.Empty();
}
