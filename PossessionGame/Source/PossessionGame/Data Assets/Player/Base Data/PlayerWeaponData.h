#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Data Assets/Base Data Assets/WeaponData.h"
#include "PlayerWeaponData.generated.h"

enum class EVariation : uint8;

UCLASS()
class POSSESSIONGAME_API UPlayerWeaponData : public UWeaponData
{
	GENERATED_BODY()
public:
    bool GetHitScanEnabled() const { return bHitScan; }
    int32 GetTotalAmmo() const { return TotalAmmo; }
    float GetHitScanDamage() const { return HitScanDamage; }
    float GetHitScanSpread() const { return HitScanSpread; }
    FVector GetFirstWeaponLocation() const { return FirstWeaponLocation; }
    FRotator GetFirstWeaponRotation() const { return FirstWeaponRotation; }
    FVector GetFirstWeaponScale() const { return FirstWeaponScale; }
    FVector GetSecondWeaponLocation() const { return SecondWeaponLocation; }
    FRotator GetSecondWeaponRotation() const { return SecondWeaponRotation; }
    FVector GetSecondWeaponScale() const { return SecondWeaponScale; }
    FVector GetAimLocation() const { return AimLocation; }
    TObjectPtr<UMaterialInterface> GetVariationMaterial(const EVariation InVariation);
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
    UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
    FVector AimLocation;
    UPROPERTY(EditAnywhere, Category = "Variation Materials", meta = (AllowPrivateAccess = true))
    TMap<EVariation, TObjectPtr<UMaterialInterface>> VariationMaterials;
};