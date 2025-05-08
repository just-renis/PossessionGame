#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PossessionGame/Data Assets/AI/Base Data/AIWeaponData.h"
#include "PossessionGame/Data Assets/Player/Base Data/PlayerWeaponData.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/Enums/EWeapon.h"
#include "PossessionGame/Interfaces/DamageReasonProvider.h"
#include "Weapon.generated.h"

enum class EVariation : uint8;
enum class EVfx : uint8;
class UPrimaryDataAssetsManager;
class AEnemy;
class UHandsAnimInstance;
class UDelegateManager;
class UNiagaraComponent;
class UAIWeaponData;
class UPlayerWeaponData;
class UWeaponAnimInstance;

UCLASS()
class POSSESSIONGAME_API AWeapon : public AActor, public IDamageReasonProvider
{
	GENERATED_BODY()
public:
	AWeapon();

	bool GetIsReloadEnabled() const { return bIsReloadEnabled; }
	bool GetIsPossessed() const { return bIsPossessed; }
	bool GetIsFirstShotWasShot() const { return bFirstShotWasShot; }
	int32 GetShootCyclesUntilReload() const { return ShootCyclesUntilReload; }
	int32 GetCurrentAmmoAmount() const { return CurrentAmmo; }
	int32 GetTotalAmmoAmount() const { return TotalAmmo; }
	int32 GetAmmoNeeded() const { return MagazineCap - CurrentAmmo; }
	int32 GetMagazineCap() const { return MagazineCap; }
	float GetShootCooldown() const { return ShootCooldown; }
	EWeapon GetWeaponName() const { return Name; }
	FVector GetSocketLocation(const FName InSocketName) const { return Mesh->GetSocketLocation(InSocketName); }
	FRotator GetSocketRotation(const FName InSocketName) const { return Mesh->GetSocketRotation(InSocketName); }
	FVector GetSocketScale(const FName InSocketName) const { return Mesh->GetSocketTransform(InSocketName).GetScale3D(); }
	FVector GetHandsSocketLocation(const FName InSocketName) const { return HandsMesh->GetSocketLocation(InSocketName); }
	FProjectileData& GetProjectileData(const EPoolObject InProjectileName) const { return bIsPossessed ? P_Data->GetProjectileData(InProjectileName) : AI_Data->GetProjectileData(InProjectileName); }
	TObjectPtr<UWeaponAnimInstance>& GetAnimInstance() { return AnimInstance; }
	TObjectPtr<UHandsAnimInstance>& GetHandsAnimInstance() { return HandsAnimInstance; }
	TObjectPtr<USkeletalMeshComponent>& GetMesh() { return Mesh; }
	TObjectPtr<UNiagaraComponent>& GetMuzzleFlashComponent() { return MuzzleFlashComponent; }
	
	virtual void AI_Shoot();
	void AI_ResetShootCycles();
	virtual void AI_Reload() {}
	
	virtual bool P_CanReload(const int32 InOtherAmmoSource = -1);
	virtual bool P_CanShoot(const bool InbIsFirstWeapon = true);
	
	void P_SetWalkingSpeed(const float InSpeed);
	virtual void P_Reload(const int32 InOtherAmmoSource = -1);
	virtual void P_Shoot(const bool bIsFirstWeapon = true);
	void P_InitialSetupTransfer();
	virtual void P_SetupTransfer(const bool bSetToFirstHand = true);
	virtual void P_UpdateAmmoTextHUD();
	void P_AttachHandsToCamera(const FVector InLocation, const FRotator InRotation, const FVector InScale = FVector::ZeroVector);
	virtual void P_AttachWeaponToHands(const TObjectPtr<USkeletalMeshComponent>& InHandsMesh, const FName InSocketName = "PlayerWeaponSocket1");
	void P_SetWalkSpread(const bool bState);
	virtual void P_SetVariationMaterial(const EVariation InVariationName);
	virtual void P_ApplyUpgradeOnSelection(const EUpgrade InUpgradeName) {}
	
	virtual void SetWeaponMeshScaleToOriginal() { Mesh->SetRelativeScale3D(OriginalScale); }
	void SetFirstShotWasShot() { bFirstShotWasShot = true; }
	virtual void DestroyThis();
	virtual void SetToOriginalSettings();
	virtual void DeactivateWeapon();
	virtual void SetProprietor(const TObjectPtr<AEnemy> InProprietor);
	void SubtractShootCycleUntilReload() { ShootCyclesUntilReload--; }
	void SetCastShadow(const bool bState) const { if (Mesh) Mesh->SetCastShadow(bState); }
	void AttachToSocket(const TObjectPtr<USkeletalMeshComponent>& OwnerMesh) const;
	virtual void ShootProjectile(const EPoolObject InProjectileName, bool bOverrideRotation = false, bool bFromFirstWeapon = true);
	virtual void PostProprietorSetup();
protected:
	bool bIsPossessed = false;
	bool bIsHitScanEnabled = false;
	bool bIsReloadEnabled = true;
	bool bWalkSpreadEnabled = false;
	int32 MagazineCap;
	int32 TotalAmmo;
	float ShootCooldown;
	float HitScanDamage;
	float ShootTimer = 0.f;
	EWeapon Name;
	FVector OriginalScale;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	int32 CurrentAmmo;
	UPROPERTY()
	TObjectPtr<UWeaponAnimInstance> AnimInstance;
	UPROPERTY()
	TObjectPtr<UHandsAnimInstance> HandsAnimInstance;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> Mesh;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> HandsMesh;
	UPROPERTY()
	TObjectPtr<UAIWeaponData> AI_Data;
	UPROPERTY()
	TObjectPtr<UPlayerWeaponData> P_Data;
	UPROPERTY()
	TObjectPtr<UDelegateManager> DelegateManager;
	UPROPERTY()
	TObjectPtr<AEnemy> Proprietor;
	UPROPERTY()
	TMap<ESound, TObjectPtr<UAudioComponent>> Sounds;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> MuzzleFlashComponent;
	UPROPERTY()
	TObjectPtr<UPrimaryDataAssetsManager> AssetsManager;

	virtual void AI_Setup();
	void AI_LoadDataIfNotLoaded(const EWeapon InPrimaryDataAssetName);
	
	float P_GetHitScanCalcSpread() const { return P_Data->GetHitScanSpread() * (bWalkSpreadEnabled ? 2.f : 1.f); }
	
	virtual void P_Setup();
	void P_LoadDataIfNotLoaded(const EWeapon InPrimaryDataAssetName);
	
	virtual void BeginPlay() override;
	virtual void Tick(const float DeltaTime) override;
	virtual void HitScanLogic();
	void MakeShootingCameraShake() const;
	virtual void SetMeshes(const EWeapon InPrimaryDataAssetName);
	void SetupHandsAnimInstance();
	virtual void DetachWeapon();
	void PlaySound(const ESound InSoundName);
	void MuzzleFlashLogic() const;
	void DisableHandsMesh() const;
	void DealPointDamage(const FHitResult& InHitResult);
private:
	bool bFirstShotWasShot = false;
	int32 ShootCyclesUntilReload;
	UPROPERTY(EditAnywhere, Category = "Camera", meta = (AllowPrivateAccess = true))
	TSubclassOf<UCameraShakeBase> ShootingCameraShake;
	
	void P_SetupUpgradesAppliance();
	void P_SetupLevelUpAppliance(const int32 InLevel);

	virtual EDamageReason GetDamageReason() const override { return P_Data->GetDamageReason(); } // It does not matter from which data i will get damage reason, since it would be the same.
};