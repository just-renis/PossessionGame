#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Data Assets/Base Data Assets/WeaponData.h"
#include "PlayerWeaponData.generated.h"

enum class EVariation : uint8;

USTRUCT()
struct FDamageDistanceEntry
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly)
    float MaxDistanceMeters = 0.f;
    UPROPERTY(EditDefaultsOnly)
    float DamageMultiplier = 1.f;
};

USTRUCT()
struct FDamageRangeEntries
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly)
    TArray<FDamageDistanceEntry> DamageDistanceEntries;
    UPROPERTY(EditDefaultsOnly)
    float MaxRangeDamageMultiplier = 1.f;

    float GetDamageMultiplier(const float DistanceMeters) const
    {
        for (const FDamageDistanceEntry Entry : DamageDistanceEntries)
        {
            if (DistanceMeters <= Entry.MaxDistanceMeters) return Entry.DamageMultiplier;
        }
        return MaxRangeDamageMultiplier;
    }
};

UCLASS()
class POSSESSIONGAME_API UPlayerWeaponData : public UWeaponData
{
	GENERATED_BODY()
public:
    bool GetHitScanEnabled() const { return bHitScan; }
    int32 GetTotalAmmo() const { return TotalAmmo; }
    float GetHitScanDamage() const { return HitScanDamage; }
    float GetHitScanSpread() const { return HitScanSpread; }
    float GetDamageMultiplierFromRange(const float InMeters) const;
    float GetVariationHueValue(const EVariation InVariation);
    FVector GetFirstWeaponLocation() const { return FirstWeaponLocation; }
    FRotator GetFirstWeaponRotation() const { return FirstWeaponRotation; }
    FVector GetFirstWeaponScale() const { return FirstWeaponScale; }
    FVector GetSecondWeaponLocation() const { return SecondWeaponLocation; }
    FRotator GetSecondWeaponRotation() const { return SecondWeaponRotation; }
    FVector GetSecondWeaponScale() const { return SecondWeaponScale; }
private:
    UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
    bool bHitScan = true;
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
    UPROPERTY(EditAnywhere, Category = "Variation Hue Values", meta = (AllowPrivateAccess = true))
    TMap<EVariation, float> VariationHueValues;
    UPROPERTY(EditAnywhere, Category = "Damage Range Entries", meta = (AllowPrivateAccess = true))
    FDamageRangeEntries DamageRangeEntries;
};