#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PossessionGame/Data Assets/AI/Base Data/AIWeaponData.h"
#include "PossessionGame/Data Assets/Player/Base Data/PlayerWeaponData.h"
#include "PossessionGame/Data Assets/Primary Data Assets/PA_WeaponContainer.h"
#include "PossessionGame/Enums/ESound.h"
#include "PossessionGame/Enums/EVfx.h"
#include "PossessionGame/Enums/EWeapon.h"
#include "Weapon.generated.h"

class UPrimaryDataAssetsManager;
class AEnemy;
class UHandsAnimInstance;
class UDelegateManager;
class UCameraComponent;
class UNiagaraComponent;
class UAIWeaponData;
class UPlayerWeaponData;
class UWeaponAnimInstance;
class UNiagaraSystem;

UCLASS()
class POSSESSIONGAME_API AWeapon : public AActor
{
	GENERATED_BODY()
public:
	AWeapon();

	bool GetIsReloadEnabled() { return bIsReloadEnabled; }
	bool GetIsPossessed() { return bIsPossessed; }
	int32 GetShootCyclesUntilReload() { return ShootCyclesUntilReload; }
	int32 GetCurrentAmmoAmount() { return CurrentAmmo; }
	int32 GetTotalAmmoAmount() { return TotalAmmo; }
	int32 GetAmmoNeeded() { return MagazineCap - CurrentAmmo; }
	int32 GetMagazineCap() { return MagazineCap; }
	float GetShootCooldown() { return ShootCooldown; }
	EWeapon GetWeaponName() { return Name; }
	FVector GetSocketLocation(const FName SocketName) { return Mesh->GetSocketLocation(SocketName); }
	FRotator GetSocketRotation(const FName SocketName) { return Mesh->GetSocketRotation(SocketName); }
	FVector GetSocketScale(const FName SocketName) { return Mesh->GetSocketTransform(SocketName).GetScale3D(); }
	FProjectileData GetProjectileData(EPoolObject ProjectileName) { return bIsPossessed ? P_Data->GetProjectileData(ProjectileName) : AI_Data->GetProjectileData(ProjectileName); }
	TObjectPtr<UWeaponAnimInstance>& GetAnimInstance() { return AnimInstance; }
	TObjectPtr<UHandsAnimInstance>& GetHandsAnimInstance() { return HandsAnimInstance; }
	TObjectPtr<USkeletalMeshComponent>& GetMesh() { return Mesh; }
	
	virtual void AI_Shoot();
	void AI_ResetShootCycles();
	virtual void AI_Reload() {}
	
	virtual bool P_CanReload(int32 OtherAmmoSource = -1);
	virtual bool P_CanShoot(bool bIsFirstWeapon = true);
	
	virtual void P_SetWalkingSpeed(float Speed);
	virtual void P_Reload(int32 OtherAmmoSource = -1);
	virtual void P_Shoot(bool bIsFirstWeapon = true);
	void P_InitialSetupTransfer();
	virtual void P_SetupTransfer(bool bSetToFirstHand = true);
	virtual void P_UpdateAmmoTextHUD();
	void P_AttachHandsToCamera(FVector Location, FRotator Rotation, FVector Scale = FVector::ZeroVector);
	virtual void P_AttachWeaponToHands(TObjectPtr<USkeletalMeshComponent> InHandsMesh, FName SocketName = "PlayerWeaponSocket1");
	void P_SetWalkSpread(bool bState);
	
	virtual void SetWeaponMeshScaleToOriginal() { Mesh->SetRelativeScale3D(OriginalScale); }
	virtual void DestroyThis();
	virtual void SetToOriginalSettings();
	virtual void DeactivateWeapon();
	virtual void SetProprietor(TObjectPtr<AEnemy> InProprietor) { Proprietor = InProprietor; }
	void SubtractShootCycleUntilReload() { ShootCyclesUntilReload--; }
	void SetCastShadow(bool bState) { if (Mesh) Mesh->SetCastShadow(bState); }
	void AttachToSocket(TObjectPtr<USkeletalMeshComponent> OwnerMesh);
	virtual void ShootProjectile(EPoolObject ProjectileName);
	void PostProprietorSetup();
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
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> LaserSight;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> Mesh;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> HandsMesh;
	UPROPERTY()
	TObjectPtr<UAIWeaponData> AI_Data;
	UPROPERTY()
	TObjectPtr<UPlayerWeaponData> P_Data;
	UPROPERTY(EditAnywhere, Category = "Particles", meta = (AllowPrivateAccess = true))
	TSoftObjectPtr<UNiagaraSystem> LaserSightSystem;
	UPROPERTY()
	TObjectPtr<UDelegateManager> DelegateManager;
	UPROPERTY()
	TObjectPtr<AEnemy> Proprietor;
	UPROPERTY()
	TMap<ESound, TObjectPtr<UAudioComponent>> Sounds;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> MuzzleFlashComponent;

	virtual void AI_Setup();
	void AI_LoadDataIfNotLoaded(EWeapon PrimaryDataAssetName);
	
	float P_GetHitScanCalcSpread() { return P_Data->GetHitScanSpread() * (bWalkSpreadEnabled ? 2.f : 1.f); }
	
	virtual void P_Setup();
	void P_LoadDataIfNotLoaded(EWeapon PrimaryDataAssetName);
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void HitScanLogic();
	void MakeCameraShake();
	virtual void LoadSounds() {}
	virtual void SetMeshes(EWeapon PrimaryDataAssetName);
	void SetupHandsAnimInstance();
	virtual void DetachWeapon();
	void PlaySound(ESound SoundName);
	void MuzzleFlashLogic();
	void DisableHandsMesh();
	void AttachMuzzleFlash(EVfx VfxName);
	void AttachAndLoadVfxs();
private:
	int32 ShootCyclesUntilReload;
	UPROPERTY(EditAnywhere, Category = "Camera", meta = (AllowPrivateAccess = true))
	TSubclassOf<UCameraShakeBase> ShootingCameraShake;
	UPROPERTY()
	TObjectPtr<UPrimaryDataAssetsManager> AssetsManager;
	
	void DeactivateLaserSight();
	void SpawnOrLoadHitScanImpactVfx(EVfx VfxName, const FVector& InLocation, const FRotator& InRotation);
	void SpawnHitScanImpactVfx(EVfx VfxName, const FVector& InLocation, const FRotator& InRotation);
	UFUNCTION()
	void LaserSightLogic();
};