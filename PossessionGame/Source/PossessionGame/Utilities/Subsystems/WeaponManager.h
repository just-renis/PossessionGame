#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Enums/EWeapon.h"
#include "WeaponManager.generated.h"

class AWeapon;

UCLASS()
class POSSESSIONGAME_API UWeaponManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	TObjectPtr<AWeapon> GetWeapon(TObjectPtr<UWorld> World, EWeapon WeaponName, TObjectPtr<USkeletalMeshComponent> Mesh, FName SocketName = "AIWeaponSocket1");
	
	void AddWeapon(EWeapon WeaponType, TSubclassOf<AWeapon> Weapon);
private:
	UPROPERTY()
	TMap<EWeapon, TSubclassOf<AWeapon>> Weapons;
};