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
	if (const TObjectPtr<UMaterialInterface>* MI_Glitching = Materials.Find(EPostProcessMaterial::Glitching))
	{
		PostProcessVolume->AddOrUpdateBlendable(*MI_Glitching, 1.f);
		GetWorld()->GetTimerManager().SetTimer(DisableGlitchingMaterialHandle, this, &UPostProcessManager::DisableGlitchingMaterial, InDuration);
	}
}

void UPostProcessManager::ToggleMaterial(const EPostProcessMaterial InMaterialName, const float InValue)
{
	if (const TObjectPtr<UMaterialInterface>* Material = Materials.Find(InMaterialName)) PostProcessVolume->AddOrUpdateBlendable(*Material, InValue);
}

void UPostProcessManager::AddOrUpdateDynamicMaterial(const EPostProcessMaterial InMaterialName, const TObjectPtr<UMaterialInstanceDynamic>& InMaterial, const float InWeight)
{
	Materials.FindOrAdd(InMaterialName, InMaterial);
	PostProcessVolume->AddOrUpdateBlendable(InMaterial, InWeight);
}

void UPostProcessManager::FadeOutDeathMaterials()
{
	FadingOutDeathMaterialsCounter = 1.f;
	GetWorld()->GetTimerManager().SetTimer(FadingOfDeathMaterialsHandle, this, &UPostProcessManager::ProcessFadingOfDeathMaterials, 0.075f, true);
}

void UPostProcessManager::DisableGlitchingMaterial()
{
	if (const TObjectPtr<UMaterialInterface>* MI_Glitching = Materials.Find(EPostProcessMaterial::Glitching)) PostProcessVolume->AddOrUpdateBlendable(*MI_Glitching, 0.f);
}

void UPostProcessManager::ProcessFadingOfDeathMaterials()
{
	FadingOutDeathMaterialsCounter -= 0.025f;
	if (const TObjectPtr<UMaterialInterface>* Material = Materials.Find(EPostProcessMaterial::DeathGlitching)) PostProcessVolume->AddOrUpdateBlendable(*Material, FadingOutDeathMaterialsCounter);
	if (const TObjectPtr<UMaterialInterface>* Material = Materials.Find(EPostProcessMaterial::DeathBloodPumping)) PostProcessVolume->AddOrUpdateBlendable(*Material, FadingOutDeathMaterialsCounter);
	if (const TObjectPtr<UMaterialInterface>* Material = Materials.Find(EPostProcessMaterial::DeathScreenCrack)) PostProcessVolume->AddOrUpdateBlendable(*Material, FadingOutDeathMaterialsCounter);
	if (FadingOutDeathMaterialsCounter <= 0.001f) GetWorld()->GetTimerManager().ClearTimer(FadingOfDeathMaterialsHandle);
}

void UPostProcessManager::Deinitialize()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(DisableGlitchingMaterialHandle);
		GetWorld()->GetTimerManager().ClearTimer(FadingOfDeathMaterialsHandle);
	}
	Super::Deinitialize();
}
