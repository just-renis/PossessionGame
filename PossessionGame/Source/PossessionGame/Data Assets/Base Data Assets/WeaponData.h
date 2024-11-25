#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PossessionGame/Utilities/Data Utilities/FProjectileData.h"
#include "WeaponData.generated.h"

enum class EPoolObject : uint8;

UCLASS()
class POSSESSIONGAME_API UWeaponData : public UDataAsset
{
	GENERATED_BODY()
public:
    bool GetIsReloadEnabled() { return bIsReloadEnabled; }
	bool GetIsCyclingEnabled() { return bIsCyclingEnabled; }
    int32 GetCurrentAmmo() { return CurrentAmmo; }
    int32 GetMagazineCap() { return MagazineCap; }
	int32 GetShootCyclesUntilReload() { return ShootCyclesUntilReload; }
    float GetShootCooldown() { return ShootCooldown; }
	FProjectileData& GetProjectileData(EPoolObject ProjectileName);

	void SetAllProjectilesWalkSpread(bool bState);
private:
    UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
    bool bIsReloadEnabled = true;
    UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
    bool bIsCyclingEnabled;
    UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
    int32 CurrentAmmo;
    UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	int32 MagazineCap;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	int32 ShootCyclesUntilReload;
    UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
    float ShootCooldown;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TMap<EPoolObject, FProjectileData> ProjectilesData;
};