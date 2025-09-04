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

UCLASS()
class POSSESSIONGAME_API ADualWieldingWeapon : public AWeapon
{
	GENERATED_BODY()
public:
    ADualWieldingWeapon();

    virtual void AI_Setup() override;
    virtual void AI_Shoot() override;

    virtual bool P_CanShoot() override;
    virtual bool P_CanReload(int32 OtherAmmoSource) override;
    
    virtual void P_Shoot() override;
    virtual void P_SetupTransfer(bool bSetToFirstHand) override;
    virtual void P_UpdateAmmoTextHUD() override;
    virtual void P_ChangeVfxVariation() override;

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

    virtual void PlayShootMontage();
	virtual void PostProprietorSetup() override;
private:
    UPROPERTY()
    FWeaponCyclingState CyclingState;
    
    virtual void P_SetVariationMaterial() override;

    virtual FVector GetSocketLocation(const FName InSocketName) const override;
    virtual FRotator GetSocketRotation(const FName InSocketName) const override;
    
    void CyclingModeLogic();
    void NonCyclingModeLogic();
    virtual void SetReverseActionSpeed(const float InActionSpeed) override;
    virtual void MuzzleFlashLogic() override;
};