#pragma once

#include "FStateTreeData.generated.h"

class UStateTreeComponent;
class AWeapon;

USTRUCT(BlueprintType)
struct POSSESSIONGAME_API FStateTreeData
{
	GENERATED_BODY()

	bool GetIsTargetInMySight() { return bIsTargetInMySight; }
	bool GetIsEQSPointAvailable() { return bIsEQSPointAvailable; }
	FVector GetTargetLocation() { return Target->GetActorLocation(); }
	TObjectPtr<AActor> GetTarget() { return Target; }
	TObjectPtr<UStateTreeComponent>& GetStateTreeRef() { return StateTree; }

	void SetIsTargetInMySight(bool bState) { bIsTargetInMySight = bState; }
	void SetReloadDelay(float Value) { ReloadDelay = Value; }
	void SetShootCooldown(float Value) { ShootCooldown = Value; }
	void SetSearchLocation(FVector Value) { SearchLocation = Value;}
	void SetStateTreeRef(TObjectPtr<UStateTreeComponent>& InStateTreeRef) { StateTree = InStateTreeRef; }
    void SetWeaponRef(TObjectPtr<AWeapon>& InWeaponRef) { Weapon = InWeaponRef; }
    void SetTargetRef(TObjectPtr<AActor>& InTargetRef) { Target = InTargetRef; }
    void SetEQSTargetRef(TObjectPtr<AActor>& InEQSTargetRef) { EQSTarget = InEQSTargetRef; }
	void SetEQSPointLocation(FVector Value);
	void SetEQSPointAvailability(bool bState) { bIsEQSPointAvailable = bState; }
private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bIsEQSPointAvailable = false;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bIsTargetInMySight = false;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bCanShoot = true;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float ReloadDelay = 0.f;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float ShootCooldown = 0.f;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	FVector SearchLocation = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<AActor> Target;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<AActor> EQSTarget;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<AWeapon> Weapon;
	UPROPERTY()
	TObjectPtr<UStateTreeComponent> StateTree;
};
