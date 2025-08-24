#include "SaveSlotManager.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/MetaSaveSlot.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/RunSaveSlot.h"
#include "PossessionGame/Utilities/Data Utilities/SaveData/SaveSlot.h"
#include "PossessionGame/Utilities/Global Utilities/Unspawnable/PossessionGameInstance.h"

bool USaveSlotManager::DoesSaveSlotExists(const FString& InSlotName) { return UGameplayStatics::DoesSaveGameExist(InSlotName, 0); }

void USaveSlotManager::LoadSaveSlot(const FString& InSaveSlotName)
{
	if (DoesSaveSlotExists(InSaveSlotName)) PlayerSaveSlot = Cast<USaveSlot>(UGameplayStatics::LoadGameFromSlot(InSaveSlotName, 0));
	else
	{
		PlayerSaveSlot = Cast<USaveSlot>(UGameplayStatics::CreateSaveGameObject(USaveSlot::StaticClass()));
		UGameplayStatics::SaveGameToSlot(PlayerSaveSlot, InSaveSlotName, 0);
	}
	PlayerSaveSlot->SetupSlot(Cast<UPossessionGameInstance>(GetGameInstance()), InSaveSlotName);
}

void USaveSlotManager::LoadRunSaveSlot(const FString& InSaveSlotName)
{
	if (DoesSaveSlotExists(InSaveSlotName)) PlayerRunSaveSlot = Cast<URunSaveSlot>(UGameplayStatics::LoadGameFromSlot(InSaveSlotName, 0));
	else
	{
		PlayerRunSaveSlot = Cast<URunSaveSlot>(UGameplayStatics::CreateSaveGameObject(URunSaveSlot::StaticClass()));
		UGameplayStatics::SaveGameToSlot(PlayerRunSaveSlot, InSaveSlotName, 0);
	}
	PlayerRunSaveSlot->SetupSlot(GetGameInstance(), InSaveSlotName);
}

void USaveSlotManager::LoadMetaSaveSlot(const FString& InSaveSlotName)
{
	if (DoesSaveSlotExists(InSaveSlotName)) MetaSaveSlot = Cast<UMetaSaveSlot>(UGameplayStatics::LoadGameFromSlot(InSaveSlotName, 0));
	else
	{
		MetaSaveSlot = Cast<UMetaSaveSlot>(UGameplayStatics::CreateSaveGameObject(UMetaSaveSlot::StaticClass()));
		UGameplayStatics::SaveGameToSlot(MetaSaveSlot, InSaveSlotName, 0);
	}
	MetaSaveSlot->SetupSlot(InSaveSlotName);
}

void USaveSlotManager::ResetSaveSlot()
{
	UGameplayStatics::DeleteGameInSlot(PlayerSaveSlot->GetSlotName(), 0);
	LoadSaveSlot(PlayerSaveSlot->GetSlotName());
}

void USaveSlotManager::ResetRunSaveSlot()
{
	UGameplayStatics::DeleteGameInSlot(PlayerRunSaveSlot->GetSlotName(), 0);
	LoadRunSaveSlot(PlayerRunSaveSlot->GetSlotName());
}
