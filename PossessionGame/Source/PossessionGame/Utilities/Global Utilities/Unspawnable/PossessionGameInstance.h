#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PossessionGameInstance.generated.h"

class URunSaveSlot;
class UPlayerLevelData;
class USaveSlot;

UCLASS()
class POSSESSIONGAME_API UPossessionGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	bool IsDemo() const { return bIsDemo; }
	bool IsPlayerTakenDamageEffectsEnabled() const { return bPlayerTakenDamageEffectsEnabled; }
	float GetTimeDilation() const { return GlobalTimeDilation; }
	int32 GetPlayerLevel() const { return PlayerLevel; }
	TObjectPtr<UPlayerLevelData> GetPlayerLevelData() const { return PlayerLevelData; }
	TObjectPtr<USaveSlot> GetSaveSlot() const { return PlayerSaveSlot; }
	TObjectPtr<URunSaveSlot> GetRunSaveSlot() const { return PlayerRunSaveSlot; }
	
	void LoadPlayerSaveSlot();
	void LoadPlayerRunSaveSlot();
	void ResetSaveSlot();
	void ResetRunSaveSlot();
private:
	UPROPERTY(EditDefaultsOnly, Category = "Editor Only Variables", meta = (AllowPrivateAccess = true))
	bool bPlayerTakenDamageEffectsEnabled = true;
	UPROPERTY(EditDefaultsOnly, Category = "Demo", meta = (AllowPrivateAccess = true))
	bool bIsDemo = true;
	UPROPERTY(EditDefaultsOnly, Category = "Game Data", meta = (AllowPrivateAccess = true))
	TObjectPtr<UPlayerLevelData> PlayerLevelData;
	UPROPERTY(EditDefaultsOnly, Category = "Editor Only Variables", meta = (AllowPrivateAccess = true))
	float GlobalTimeDilation = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Editor Only Variables", meta = (AllowPrivateAccess = true))
	int32 PlayerLevel = 0;
	UPROPERTY()
	TObjectPtr<USaveSlot> PlayerSaveSlot;
	UPROPERTY()
	TObjectPtr<URunSaveSlot> PlayerRunSaveSlot;

	virtual void Init() override;
};