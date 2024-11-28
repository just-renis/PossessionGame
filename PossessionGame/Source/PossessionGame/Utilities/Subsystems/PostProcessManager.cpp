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

void UPostProcessManager::TriggerGlitchingMaterial(const float InDuration)
{
	if (Materials.Contains(EPostProcessMaterial::Glitching))
	{
		PostProcessVolume->AddOrUpdateBlendable(Materials[EPostProcessMaterial::Glitching], 1.f);
		UKismetSystemLibrary::K2_SetTimer(this, "DisableGlitchingEffect", InDuration, false);
	}
}

void UPostProcessManager::DisableGlitchingEffect()
{
	if (Materials.Contains(EPostProcessMaterial::Glitching)) PostProcessVolume->AddOrUpdateBlendable(Materials[EPostProcessMaterial::Glitching], 0.f);
}
