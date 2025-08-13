#include "PossessionGameInstance.h"
#include "PossessionGameUserSettings.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/MetaSaveSlot.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/SaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/PlayerDataManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"
#include "PossessionGame/Utilities/Subsystems/SaveSlotManager.h"

void UPossessionGameInstance::InitGameInstanceManagers()
{
	GetTimerManager().SetTimer(TickHandle, this, &UPossessionGameInstance::Tick, 0.1f, true);
	GetSubsystem<UPrimaryDataAssetsManager>()->Init(GetSubsystem<USaveSlotManager>()->GetSaveSlot()->GetIsTutorialFinished(), SpawnableEnemiesByTotalAmount);
}

void UPossessionGameInstance::Init()
{
	Super::Init();
	GetSubsystem<USaveSlotManager>()->LoadMetaSaveSlot("MetaSaveSlot");
	const TObjectPtr<UPossessionGameUserSettings> GameUserSettings = Cast<UPossessionGameUserSettings>(GEngine->GetGameUserSettings());
	if (GetSubsystem<USaveSlotManager>()->GetMetaSaveSlot()->GetIsFirstLaunch())
	{
		GameUserSettings->LoadSettings(false);
		GameUserSettings->SetFrameRateLimit(60.f);
		GameUserSettings->ApplySettings(true);
	}
	else GameUserSettings->LoadSettings(false);
}

void UPossessionGameInstance::Tick() const { GetSubsystem<UPlayerDataManager>()->Tick(TickTime); }

void UPossessionGameInstance::Shutdown()
{
	GetTimerManager().ClearTimer(TickHandle);
	Super::Shutdown();
}
