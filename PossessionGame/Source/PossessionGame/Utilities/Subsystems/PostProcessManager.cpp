#include "PostProcessManager.h"
#include "Kismet/GameplayStatics.h"

void UPostProcessManager::Init(const TMap<EPostProcessMaterial, TObjectPtr<UMaterialInterface>>& InPostProcessMaterials)
{
	if (const TObjectPtr<AActor> FoundPostProcessVolumeActor = UGameplayStatics::GetActorOfClass(GetWorld(), APostProcessVolume::StaticClass()))
	{
		PostProcessVolume = Cast<APostProcessVolume>(FoundPostProcessVolumeActor);
		Materials = InPostProcessMaterials;
	}
}

void UPostProcessManager::ToggleMaterial(const EPostProcessMaterial InMaterialName, const float InValue)
{
	if (Materials.Contains(InMaterialName)) PostProcessVolume->AddOrUpdateBlendable(Materials[InMaterialName], InValue);
}
