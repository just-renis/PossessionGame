#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PossessionGame/Enums/EHandsAnimState.h"
#include "PossessionGame/Enums/EWeaponAnimState.h"
#include "PA_WeaponContainer.generated.h"

class AWeapon;
class AProjectile;
class UAIWeaponData;
class UPlayerWeaponData;

UCLASS()
class POSSESSIONGAME_API UPA_WeaponContainer : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Weapon Data")
	FName WeaponName;
	UPROPERTY(EditAnywhere, Category = "Weapon Data")
	TSoftClassPtr<AWeapon> WeaponClass;
	UPROPERTY(EditAnywhere, Category = "Weapon Data")
	TSoftObjectPtr<USkeletalMesh> WeaponMesh;
	UPROPERTY(EditAnywhere, Category = "Weapon Data")
	TSoftObjectPtr<USkeletalMesh> HandsMesh;
	UPROPERTY(EditAnywhere, Category = "Weapon Data")
	TSoftObjectPtr<UAIWeaponData> AI_Data;
	UPROPERTY(EditAnywhere, Category = "Weapon Data")
	TSoftObjectPtr<UPlayerWeaponData> P_Data;
	UPROPERTY(EditAnywhere, Category = "Weapon Animation Montages")
	TMap<EWeaponAnimState, TSoftObjectPtr<UAnimMontage>> WeaponMontages;
	UPROPERTY(EditAnywhere, Category = "Hands Animation Montages")
	TMap<EHandsAnimState, TSoftObjectPtr<UAnimMontage>> HandsMontages;
	
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
