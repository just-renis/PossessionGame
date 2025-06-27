#include "MainMenuBase.h"
#include "SettingsSave.h"
#include "ShaderPipelineCache.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/UI/MainWidgets/MainMenuUI.h"
#include "PossessionGame/Utilities/Subsystems/WidgetManager.h"

void AMainMenuBase::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Error, TEXT("MainMenuLevel Total PSO Precompiles: %i"), FShaderPipelineCache::NumPrecompilesRemaining());
	GetWorld()->GetTimerManager().SetTimer(PSOPrecachingHandle, [this]
	{
		const int32 Remaining = FShaderPipelineCache::NumPrecompilesRemaining();
		UE_LOG(LogTemp, Log, TEXT("MainMenuLevel Remaining PSO Precompiles: %d"), Remaining);

		if (Remaining == 0)
		{
			GetWorld()->GetTimerManager().ClearTimer(PSOPrecachingHandle);
			InitAfterPSOPrecache();
		}
	}, 0.05f, true);
}

void AMainMenuBase::LoadSettingsData() const
{
	const FString SettingsSlot = "SettingsSlot";
	if (UGameplayStatics::DoesSaveGameExist(SettingsSlot, 0))
	{
		const TObjectPtr<USettingsSave> SavedSettings = Cast<USettingsSave>(UGameplayStatics::LoadGameFromSlot(SettingsSlot, 0));
		SavedSettings->LoadSettings(GetWorld());
	}
	else
	{
		const TObjectPtr<USettingsSave> SavedSettings = Cast<USettingsSave>(UGameplayStatics::CreateSaveGameObject(SettingsSaveClass));
		SavedSettings->LoadSettingsOnFirstLaunch(GetWorld());
		UGameplayStatics::SaveGameToSlot(SavedSettings, SettingsSlot, 0);
	}
}

void AMainMenuBase::InitAfterPSOPrecache() const
{
	LoadSettingsData();
	const TObjectPtr<UWidgetManager> WidgetManager = GetWorld()->GetSubsystem<UWidgetManager>();
	WidgetManager->Init(WidgetClasses);
	WidgetManager->GetWidget<UMainMenuUI>(EWidget::MainMenuUI)->OpenUI();
}
