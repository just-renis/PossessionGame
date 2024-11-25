#include "PossessionGameInstance.h"
#include "PossessionGameUserSettings.h"

void UPossessionGameInstance::Init()
{
	Super::Init();
	Cast<UPossessionGameUserSettings>(GEngine->GetGameUserSettings())->LoadSettings();
}
