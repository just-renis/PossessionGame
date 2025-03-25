#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Weapons/Management/Weapon.h"
#include "DualWieldingWeapon.generated.h"

class AOneHandedWeapon;

USTRUCT()
struct FWeaponCyclingState
{
    GENERATED_BODY()

    bool bIsCycling = false;
    bool bIsFirstWeaponMode = false;
};

USTRUCT()
struct FShootMontageHandler
{
    GENERATED_BODY()

    bool bIsFirstWeaponShooting = false;
    bool bIsSecondWeaponShooting = false;
};

UCLASS()
class POSSESSIONGAME_API ADualWieldingWeapon : public AWeapon
{
	GENERATED_BODY()
public:
    ADualWieldingWeapon();

    virtual void AI_Setup() override;
    virtual void AI_Shoot() override;

    virtual bool P_CanShoot(bool bIsFirstWeapon) override;
    virtual bool P_CanReload(int32 OtherAmmoSource) override;
    
    virtual void P_Shoot(bool bIsFirstWeapon) override;
    virtual void P_Reload(int32 OtherAmmoSource) override;
    virtual void P_SetupTransfer(bool bSetToFirstHand) override;
    virtual void P_UpdateAmmoTextHUD() override;
    void P_SetShootMontage(const bool bFirstWeapon, const bool bState);

    virtual void SetWeaponMeshScaleToOriginal() override;
    virtual void DestroyThis() override;
    virtual void SetToOriginalSettings() override;
    virtual void DeactivateWeapon() override;
    virtual void SetupOneHandedWeapons(TObjectPtr<USkeletalMeshComponent> CharacterMesh) {}
    virtual void SetProprietor(TObjectPtr<AEnemy> InProprietor) override;
protected:
    UPROPERTY()
    TObjectPtr<AWeapon> FirstWeapon;
    UPROPERTY()
    TObjectPtr<AWeapon> SecondWeapon;
    UPROPERTY()
    FShootMontageHandler ShootMontageHandler;

    virtual void PlayShootMontage();
private:
    UPROPERTY()
    FWeaponCyclingState CyclingState;

    void CyclingModeLogic();
    void NonCyclingModeLogic();
};