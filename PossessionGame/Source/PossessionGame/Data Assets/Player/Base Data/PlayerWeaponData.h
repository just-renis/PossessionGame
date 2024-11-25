#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Data Assets/Base Data Assets/WeaponData.h"
#include "PlayerWeaponData.generated.h"

UCLASS()
class POSSESSIONGAME_API UPlayerWeaponData : public UWeaponData
{
	GENERATED_BODY()
public:
    bool GetHitScanEnabled() { return bHitScan; }
    bool GetLaserSightEnabled() { return bLaserSight; }
    int32 GetTotalAmmo() { return TotalAmmo; }
    float GetHitScanDamage() { return HitScanDamage; }
    float GetHitScanSpread() { return HitScanSpread; }
    FVector GetFirstWeaponLocation() { return FirstWeaponLocation; }
    FRotator GetFirstWeaponRotation() { return FirstWeaponRotation; }
    FVector GetFirstWeaponScale() { return FirstWeaponScale; }
    FVector GetSecondWeaponLocation() { return SecondWeaponLocation; }
    FRotator GetSecondWeaponRotation() { return SecondWeaponRotation; }
    FVector GetSecondWeaponScale() { return SecondWeaponScale; }
    FVector GetAimLocation() { return AimLocation; }
private:
    UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
    bool bHitScan = true;
    UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
    bool bLaserSight;
    UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
    int32 TotalAmmo;
    UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
    float HitScanDamage;
    UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
    float HitScanSpread;
    UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
    FVector FirstWeaponLocation;
    UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
    FRotator FirstWeaponRotation;
    UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
    FVector FirstWeaponScale;
    UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
    FVector SecondWeaponLocation;
    UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
    FRotator SecondWeaponRotation;
    UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
    FVector SecondWeaponScale;
    UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
    FVector AimLocation;
};