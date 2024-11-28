#include "PostProcessManager.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Enums/EPostProcessMaterial.h"

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

void UPostProcessManager::ToggleMaterial(const EPostProcessMaterial InMaterialName, const float InValue)
{
	if (Materials.Contains(InMaterialName)) PostProcessVolume->AddOrUpdateBlendable(Materials[InMaterialName], InValue);
}

void UPostProcessManager::DisableGlitchingEffect()
{
	if (Materials.Contains(EPostProcessMaterial::Glitching)) PostProcessVolume->AddOrUpdateBlendable(Materials[EPostProcessMaterial::Glitching], 0.f);
}
