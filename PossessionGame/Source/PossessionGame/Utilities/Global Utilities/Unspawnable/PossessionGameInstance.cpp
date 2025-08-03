#include "PossessionGameInstance.h"
#include "PossessionGameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Utilities/Data Utilities/RunSaveSlot.h"
#include "PossessionGame/Utilities/Data Utilities/SaveSlot.h"
#include "PossessionGame/Utilities/Subsystems/DelegateManager.h"
#include "PossessionGame/Utilities/Subsystems/PlayerDataManager.h"
#include "PossessionGame/Utilities/Subsystems/PrimaryDataAssetsManager.h"

void UPossessionGameInstance::LoadPlayerSaveSlot()
{
	const FString PlayerSaveSlotName = bIsDemo ? "DemoPlayerSaveSlot" : "PlayerSaveSlot";
	if (UGameplayStatics::DoesSaveGameExist(PlayerSaveSlotName, 0))
	{
		PlayerSaveSlot = Cast<USaveSlot>(UGameplayStatics::LoadGameFromSlot(PlayerSaveSlotName, 0));
	}
	else
	{
		PlayerSaveSlot = Cast<USaveSlot>(UGameplayStatics::CreateSaveGameObject(USaveSlot::StaticClass()));
		UGameplayStatics::SaveGameToSlot(PlayerSaveSlot, PlayerSaveSlotName, 0);
	}
	PlayerSaveSlot->SetupSlot(this, PlayerSaveSlotName);
}

void UPossessionGameInstance::LoadPlayerRunSaveSlot()
{
	const FString PlayerRunSaveSlotName = bIsDemo ? "DemoPlayerRunSaveSlot" : "PlayerRunSaveSlot";
	if (UGameplayStatics::DoesSaveGameExist(PlayerRunSaveSlotName, 0))
	{
		PlayerRunSaveSlot = Cast<URunSaveSlot>(UGameplayStatics::LoadGameFromSlot(PlayerRunSaveSlotName, 0));
	}
	else
	{
		PlayerRunSaveSlot = Cast<URunSaveSlot>(UGameplayStatics::CreateSaveGameObject(URunSaveSlot::StaticClass()));
		UGameplayStatics::SaveGameToSlot(PlayerRunSaveSlot, PlayerRunSaveSlotName, 0);
	}
	PlayerRunSaveSlot->SetupSlot(PlayerRunSaveSlotName);
}

void UPossessionGameInstance::ResetSaveSlot()
{
	UGameplayStatics::DeleteGameInSlot(bIsDemo ? "DemoPlayerSaveSlot" : "PlayerSaveSlot", 0);
	LoadPlayerSaveSlot();
}

void UPossessionGameInstance::ResetRunSaveSlot()
{
	UGameplayStatics::DeleteGameInSlot(bIsDemo ? "DemoPlayerRunSaveSlot" : "PlayerRunSaveSlot", 0);
	LoadPlayerRunSaveSlot();
}

void UPossessionGameInstance::InitGameInstanceManagers()
{
	GetTimerManager().SetTimer(TickHandle, this, &UPossessionGameInstance::Tick, 0.1f, true);
	GetSubsystem<UPrimaryDataAssetsManager>()->Init(PlayerSaveSlot->GetIsTutorialFinished(), MarkerClass, GetSubsystem<UDelegateManager>(), SpawnableEnemiesByTotalAmount, ToSpawnAfterTutorial);
}

void UPossessionGameInstance::Init()
{
	Super::Init();
	#if WITH_EDITOR
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), GlobalTimeDilation);
	#endif
	LoadPlayerSaveSlot();
	LoadPlayerRunSaveSlot();
	const TObjectPtr<UPossessionGameUserSettings> GameUserSettings = Cast<UPossessionGameUserSettings>(GEngine->GetGameUserSettings());
	if (PlayerSaveSlot->GetIsFirstLaunch())
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
