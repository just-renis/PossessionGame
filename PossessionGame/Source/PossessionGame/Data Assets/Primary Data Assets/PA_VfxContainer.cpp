#include "PA_VfxContainer.h"
#include "Engine/AssetManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

void UPA_VfxContainer::LoadVfx(const TObjectPtr<UWorld>& InWorld, TFunction<void()> OnVfxLoaded)
{
	World = InWorld;
	if (!bIsInLoad)
	{
		bIsInLoad = true;
		TObjectPtr<UPrimaryDataAssetsManager> AssetsManager = World->GetSubsystem<UPrimaryDataAssetsManager>();
		if (AssetsManager->GetCanAsyncLoadHappen())
		{
			TSharedPtr<FStreamableHandle> Handle = UAssetManager::GetStreamableManager().RequestAsyncLoad(VfxEffect.ToSoftObjectPath(), [this, OnVfxLoaded]
			{
				if (IsGameRunning())
				{
					bIsLoaded = true;
					OnVfxLoaded();
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