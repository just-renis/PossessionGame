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
		UKismetSystemLibrary::K2_SetTimer(this, "DisableGlitchingMaterial", InDuration, false);
	}
}

void UPostProcessManager::ToggleMaterial(const EPostProcessMaterial InMaterialName, const float InValue)
{
	if (Materials.Contains(InMaterialName)) PostProcessVolume->AddOrUpdateBlendable(Materials[InMaterialName], InValue);
}

void UPostProcessManager::FadeOutDeathMaterials()
{
	FadingOutDeathMaterialsCounter = 1.f;
	UKismetSystemLibrary::K2_SetTimer(this, "ProcessFadingOfDeathMaterials", 0.075f, true);
}

void UPostProcessManager::DisableGlitchingMaterial()
{
	if (Materials.Contains(EPostProcessMaterial::Glitching)) PostProcessVolume->AddOrUpdateBlendable(Materials[EPostProcessMaterial::Glitching], 0.f);
}

void UPostProcessManager::ProcessFadingOfDeathMaterials()
{
	FadingOutDeathMaterialsCounter -= 0.025f;
	if (Materials.Contains(EPostProcessMaterial::DeathGlitching)) PostProcessVolume->AddOrUpdateBlendable(Materials[EPostProcessMaterial::DeathGlitching], FadingOutDeathMaterialsCounter);
	if (Materials.Contains(EPostProcessMaterial::DeathBloodPumping)) PostProcessVolume->AddOrUpdateBlendable(Materials[EPostProcessMaterial::DeathBloodPumping], FadingOutDeathMaterialsCounter);
	if (Materials.Contains(EPostProcessMaterial::DeathScreenCrack)) PostProcessVolume->AddOrUpdateBlendable(Materials[EPostProcessMaterial::DeathScreenCrack], FadingOutDeathMaterialsCounter);
	if (FadingOutDeathMaterialsCounter <= 0.001f) UKismetSystemLibrary::K2_ClearTimer(this, "ProcessFadingOfDeathMaterials");
}
