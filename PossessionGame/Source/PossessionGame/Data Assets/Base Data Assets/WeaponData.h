#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PossessionGame/Utilities/Data Utilities/FProjectileData.h"
#include "PossessionGame/Utilities/Data Utilities/FNiagaraVariationData.h"
#include "WeaponData.generated.h"

enum class EPoolObject : uint8;

UCLASS()
class POSSESSIONGAME_API UWeaponData : public UDataAsset
{
	GENERATED_BODY()
public:
    bool GetIsReloadEnabled() const { return bIsReloadEnabled; }
	bool GetIsCyclingEnabled() const { return bIsCyclingEnabled; }
    int32 GetCurrentAmmo() const { return CurrentAmmo; }
    int32 GetMagazineCap() const { return MagazineCap; }
	int32 GetShootCyclesUntilReload() const { return ShootCyclesUntilReload; }
    float GetShootCooldown() const { return ShootCooldown; }
	EDamageReason GetDamageReason() const { return DamageReason; }
	FProjectileData& GetProjectileData(EPoolObject ProjectileName);
    FNiagaraVariationData GetNiagaraVariationData(const EVariation InVariation);

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
	EDamageReason DamageReason;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TMap<EPoolObject, FProjectileData> ProjectilesData;
	UPROPERTY(EditAnywhere, Category = "Vfx Variation Data", meta = (AllowPrivateAccess = true))
	TMap<EVariation, FNiagaraVariationData> VfxVariationData;
};