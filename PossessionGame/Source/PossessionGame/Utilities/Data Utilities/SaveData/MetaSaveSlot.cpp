#include "MetaSaveSlot.h"
#include "Kismet/GameplayStatics.h"

FText UMetaSaveSlot::GetSaveSlotMetaDataText(const FString& InSaveSlotName)
{
	if (const FSaveSlotMetaData* SaveSlotMetaData = SaveSlotsMetaData.Find(InSaveSlotName)) return FText::FromString(SaveSlotMetaData->GetMetaDataText());
	return FText::GetEmpty();
}

void UMetaSaveSlot::SetLaunchedOnce()
{
	bIsFirstLaunch = false;
	SaveSlot();
}

void UMetaSaveSlot::SetupSlot(const FString& InSlotName) { SlotName = InSlotName; }

void UMetaSaveSlot::UpdateSaveSlotMetaData(const FString& InSlotName, const FSaveSlotMetaData& InSaveSlotMetaData)
{
	FSaveSlotMetaData& SlotData = SaveSlotsMetaData.FindOrAdd(InSlotName);
	SlotData = InSaveSlotMetaData;
	SaveSlot();
}

void UMetaSaveSlot::SaveSlot() { UGameplayStatics::SaveGameToSlot(this, SlotName, 0); }
