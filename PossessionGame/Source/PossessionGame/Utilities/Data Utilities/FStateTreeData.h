#pragma once

#include "FStateTreeData.generated.h"

class UStateTreeComponent;
class AWeapon;

USTRUCT(BlueprintType)
struct POSSESSIONGAME_API FStateTreeData
{
	GENERATED_BODY()

	bool GetIsTargetInMySight() const { return bIsTargetInMySight; }
	bool GetIsEQSPointAvailable() const { return bIsEQSPointAvailable; }
	bool GetIsActionInMotion() const { return bIsActionInMotion; }
	FVector GetTargetLocation() const { return Target->GetActorLocation(); }
	TObjectPtr<AActor> GetTarget() { return Target; }
	TObjectPtr<UStateTreeComponent>& GetStateTreeRef() { return StateTree; }

	void SetIsActionInMotion(const bool bState) { bIsActionInMotion = bState; }
	void SetIsTargetInMySight(const bool bState) { bIsTargetInMySight = bState; }
	void SetReloadDelay(const float InValue) { ReloadDelay = InValue; }
	void SetShootCooldown(const float InValue) { ShootCooldown = InValue; }
	void SetSearchLocation(const FVector InValue) { SearchLocation = InValue;}
	void SetStateTreeRef(const TObjectPtr<UStateTreeComponent>& InStateTreeRef) { StateTree = InStateTreeRef; }
    void SetWeaponRef(const TObjectPtr<AWeapon>& InWeaponRef) { Weapon = InWeaponRef; }
    void SetTargetRef(const TObjectPtr<AActor>& InTargetRef) { Target = InTargetRef; }
    void SetEQSTargetRef(const TObjectPtr<AActor>& InEQSTargetRef) { EQSTarget = InEQSTargetRef; }
	void SetEQSPointLocation(const FVector InValue);
	void SetEQSPointAvailability(const bool bState) { bIsEQSPointAvailable = bState; }
private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bIsActionInMotion = false;
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
