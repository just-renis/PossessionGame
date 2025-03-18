#pragma once

#include "CoreMinimal.h"
#include "WeaponManager.generated.h"

enum class EWeapon : uint8;
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