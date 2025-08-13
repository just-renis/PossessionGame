#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SaveSlotManager.generated.h"

class UMetaSaveSlot;
class URunSaveSlot;
class USaveSlot;

constexpr int32 CURRENT_SAVE_VERSION = 2;

UCLASS()
class POSSESSIONGAME_API USaveSlotManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	bool DoesSaveSlotExists(const FString& InSlotName);
	TObjectPtr<USaveSlot> GetSaveSlot() { return PlayerSaveSlot; }
	TObjectPtr<URunSaveSlot> GetRunSaveSlot() { return PlayerRunSaveSlot; }
	TObjectPtr<UMetaSaveSlot> GetMetaSaveSlot() { return MetaSaveSlot; }
	
	void LoadSaveSlot(const FString& InSaveSlotName);
	void LoadRunSaveSlot(const FString& InSaveSlotName);
	void LoadMetaSaveSlot(const FString& InSaveSlotName);
	void ResetSaveSlot();
	void ResetRunSaveSlot();
private:
	UPROPERTY()
	TObjectPtr<USaveSlot> PlayerSaveSlot;
	UPROPERTY()
	TObjectPtr<URunSaveSlot> PlayerRunSaveSlot;
	UPROPERTY()
	TObjectPtr<UMetaSaveSlot> MetaSaveSlot;
};
