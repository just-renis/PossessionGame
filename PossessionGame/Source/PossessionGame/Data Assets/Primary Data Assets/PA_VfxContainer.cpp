#include "PA_VfxContainer.h"
#include "Engine/AssetManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

void UPA_VfxContainer::LoadVfx(const TObjectPtr<UWorld>& InWorld, const TFunction<void()>& OnVfxLoaded)
{
	World = InWorld;
	VfxLoadedCallbacks.Add(FTFunctionCallback(OnVfxLoaded));
	if (!bIsInLoad)
	{
		bIsInLoad = true;
		const TObjectPtr<UPrimaryDataAssetsManager> AssetsManager = World->GetSubsystem<UPrimaryDataAssetsManager>();
		if (AssetsManager->GetCanAsyncLoadHappen())
		{
			TSharedPtr<FStreamableHandle> Handle = UAssetManager::GetStreamableManager().RequestAsyncLoad(VfxEffect.ToSoftObjectPath(), [this]
			{
				if (IsGameRunning())
				{
					bIsLoaded = true;
					for (const FTFunctionCallback Callback : VfxLoadedCallbacks) if (Callback.Function) Callback.Function();
					VfxLoadedCallbacks.Empty();
				}
			});
			AssetsManager->AddAsyncHandle(Handle);
		}
	}
}

FPrimaryAssetId UPA_VfxContainer::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(TEXT("Vfx"), VfxPrimaryName);
}