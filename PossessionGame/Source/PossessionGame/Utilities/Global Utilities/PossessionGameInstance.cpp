#include "PossessionGameInstance.h"
#include "PossessionGameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "PossessionGame/Utilities/Data Utilities/SaveSlot.h"

void UPossessionGameInstance::LoadPlayerSaveSlot()
{
	const FString PlayerSaveSlotName = "PlayerSaveSlot";
	if (UGameplayStatics::DoesSaveGameExist(PlayerSaveSlotName, 0))
	{
		PlayerSaveSlot = Cast<USaveSlot>(UGameplayStatics::LoadGameFromSlot(PlayerSaveSlotName, 0));
	}
	else
	{
		PlayerSaveSlot = Cast<USaveSlot>(UGameplayStatics::CreateSaveGameObject(USaveSlot::StaticClass()));
		UGameplayStatics::SaveGameToSlot(PlayerSaveSlot, PlayerSaveSlotName, 0);
	}
	PlayerSaveSlot->SetGameInstance(this);
}

void UPossessionGameInstance::Init()
{
	Super::Init();
	LoadPlayerSaveSlot();
	Cast<UPossessionGameUserSettings>(GEngine->GetGameUserSettings())->LoadSettings();
}
