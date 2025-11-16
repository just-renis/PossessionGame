#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PossessionGame/Enums/ECharacterType.h"
#include "PossessionGameInstance.generated.h"

class UCodexData;
enum class EUpgrade : uint8;
class USaveSlot;
struct FLoadLevelData;
class UPlayerLevelData;

USTRUCT()
struct POSSESSIONGAME_API FSpawnableEnemyByTotalAmount
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	int32 SpawnThreshold = 0;
	UPROPERTY(EditDefaultsOnly)
	ECharacterType Character = ECharacterType::Random;
};

USTRUCT()
struct FCharacterUpgrades
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TArray<EUpgrade> Upgrades;
};

UCLASS()
class POSSESSIONGAME_API UPossessionGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	bool IsPlayerTakenDamageEffectsEnabled() const { return bPlayerTakenDamageEffectsEnabled; }
	bool GetIsMainMenuLaunchedOnce() const { return bMainMenuLaunchedOnce; }
	bool GetUseCustomLevel() const { return bUseCustomLevel; }
	float GetGlobalTimeDilation() const { return GlobalTimeDilation; }
	int32 GetPlayerLevel() const { return PlayerLevel; }
	TObjectPtr<UPlayerLevelData> GetPlayerLevelData() const { return PlayerLevelData; }
	
	void InitGameInstanceManagers();
	void SetMainMenuLaunchedOnce() { bMainMenuLaunchedOnce = true;}
private:
	bool bMainMenuLaunchedOnce = false;
	float TickTime = 0.1f;
	UPROPERTY(EditDefaultsOnly, Category = "Editor Only Variables", meta = (AllowPrivateAccess = true))
	bool bPlayerTakenDamageEffectsEnabled = true;
	UPROPERTY(EditDefaultsOnly, Category = "Demo", meta = (AllowPrivateAccess = true))
	bool bIsDemo = true;
	UPROPERTY(EditDefaultsOnly, Category = "Game Data", meta = (AllowPrivateAccess = true))
	TObjectPtr<UPlayerLevelData> PlayerLevelData;
	UPROPERTY(EditAnywhere, Category = "Game Data", meta = (AllowPrivateAccess = true))
	TObjectPtr<UCodexData> CodexData;
	UPROPERTY(EditDefaultsOnly, Category = "Editor Only Variables", meta = (AllowPrivateAccess = true))
	float GlobalTimeDilation = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Editor Only Variables", meta = (AllowPrivateAccess = true))
	bool bUseCustomLevel = false;
	UPROPERTY(EditDefaultsOnly, Category = "Editor Only Variables", meta = (AllowPrivateAccess = true))
	int32 PlayerLevel = 0;
	UPROPERTY(EditDefaultsOnly, Category = "Upgrades Data", meta = (AllowPrivateAccess = true))
	TMap<ECharacterType, FCharacterUpgrades> ExistingUpgrades;
	UPROPERTY(EditAnywhere, Category = "Game Modes Manager", meta = (AllowPrivateAccess = true))
	TArray<FSpawnableEnemyByTotalAmount> SpawnableEnemiesByTotalAmount;
	UPROPERTY()
	FTimerHandle TickHandle;
	
	bool IsDemo() const { return bIsDemo; }

	virtual void Init() override;
	void Tick() const;
	virtual void Shutdown() override;
	void OpenLevel(const FLoadLevelData InLoadLevelData) const;
	void AddAvailableUpgrades(const TObjectPtr<USaveSlot>& InSaveSlot) const;
};