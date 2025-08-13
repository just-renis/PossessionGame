#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MetaSaveSlot.generated.h"

USTRUCT()
struct FSaveSlotMetaData
{
	GENERATED_BODY()

	FSaveSlotMetaData() : CurrentPlayerLevel(0), SaveSlotName(TEXT("")), LastSaveDateTime(FDateTime(0)) {}
	FSaveSlotMetaData(const int32 InCurrentPlayerLevel, const FString& InSaveSlotName, const FDateTime& InDateTime)
		: CurrentPlayerLevel(InCurrentPlayerLevel), SaveSlotName(InSaveSlotName), LastSaveDateTime(InDateTime) {}

	UPROPERTY()
	int32 CurrentPlayerLevel;
	UPROPERTY()
	FString SaveSlotName;
	UPROPERTY()
	FDateTime LastSaveDateTime;

	FString GetLastSavedDate() const { return LastSaveDateTime.ToString(TEXT("%Y-%m-%d")); }
	FString GetMetaDataText() const
	{
		return NSLOCTEXT("ST_Localization", "Save", "Save ").ToString() + SaveSlotName.Right(1) + '\n' + GetLastSavedDate() + '\n' +
			FText::Format(NSLOCTEXT("ST_Localization", "Level", "Level {0}"), FText::AsNumber(CurrentPlayerLevel)).ToString();
	}
};

UCLASS()
class POSSESSIONGAME_API UMetaSaveSlot : public USaveGame
{
	GENERATED_BODY()
public:
	bool GetIsFirstLaunch() const { return bIsFirstLaunch; }
	FText GetSaveSlotMetaDataText(const FString& InSaveSlotName);
	
	void SetLaunchedOnce();
	void SetupSlot(const FString& InSlotName);
	void UpdateSaveSlotMetaData(const FString& InSlotName, const FSaveSlotMetaData& InSaveSlotMetaData);
private:
	UPROPERTY()
	bool bIsFirstLaunch = true;
	UPROPERTY()
	FString SlotName;
	UPROPERTY()
	TMap<FString, FSaveSlotMetaData> SaveSlotsMetaData;

	void SaveSlot();
};
