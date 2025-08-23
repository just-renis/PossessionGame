#pragma once

#include "FLoadLevelData.generated.h"

USTRUCT()
struct FLoadLevelData
{
	GENERATED_BODY()

	FLoadLevelData() : bSaveSlot(false), bResetSaveSlot(false), bResetRunData(false) { }
	FLoadLevelData(const bool bInSaveSlot, const bool bInResetSaveSlot, const bool bInResetRunData, const FName& InLevelName)
		: bSaveSlot(bInSaveSlot), bResetSaveSlot(bInResetSaveSlot), bResetRunData(bInResetRunData), LevelName(InLevelName) {}

	bool bSaveSlot;
	bool bResetSaveSlot;
	bool bResetRunData;
	FName LevelName;
};
