#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PossessionGameInstance.generated.h"

class UPlayerLevelData;
class USaveSlot;

UCLASS()
class POSSESSIONGAME_API UPossessionGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	float GetTimeDilation() const { return GlobalTimeDilation; }
	int32 GetPlayerLevel() const { return PlayerLevel; }
	TObjectPtr<UPlayerLevelData> GetPlayerLevelData() const { return PlayerLevelData; }
	TObjectPtr<USaveSlot> GetSaveSlot() const { return PlayerSaveSlot; };
	
	void LoadPlayerSaveSlot();
private:
	UPROPERTY(EditDefaultsOnly, Category = "Game Data", meta = (AllowPrivateAccess = true))
	TObjectPtr<UPlayerLevelData> PlayerLevelData;
	UPROPERTY(EditDefaultsOnly, Category = "Editor Only Variables", meta = (AllowPrivateAccess = true))
	float GlobalTimeDilation = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Editor Only Variables", meta = (AllowPrivateAccess = true))
	int32 PlayerLevel = 0;
	UPROPERTY()
	TObjectPtr<USaveSlot> PlayerSaveSlot;

	virtual void Init() override;
};