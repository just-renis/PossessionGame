#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Weapons/Categories/OneHandedWeapon.h"
#include "SamuraiWeapon.generated.h"

UCLASS()
class POSSESSIONGAME_API ASamuraiWeapon : public AOneHandedWeapon
{
	GENERATED_BODY()
public:
	virtual void AI_Shoot() override;
	void AI_StopProcessingMeleeAttack();

	void P_TriggerSlashVfx();
private:
	bool bIsFirstCheck = true;
	FVector LastSocketLocation;
	FTimerHandle ProcessMeleeAttackHandle;
	UPROPERTY(EditDefaultsOnly, Category = "Melee Attack", meta = (AllowPrivateAccess = true))
	float AI_DefaultDamage = 10.f;
	UPROPERTY(EditDefaultsOnly, Category = "Melee Attack", meta = (AllowPrivateAccess = true))
	float P_DefaultDamage = 20.f;
	UPROPERTY(EditDefaultsOnly, Category = "Melee Attack", meta = (AllowPrivateAccess = true))
	float P_MeleeAttackRadiusInMeters = 3.f;
	UPROPERTY(EditDefaultsOnly, Category = "Melee Attack", meta = (AllowPrivateAccess = true))
	float AttackHeight = 50.f;
	UPROPERTY(EditDefaultsOnly, Category = "Melee Attack", meta = (AllowPrivateAccess = true))
	float AttackWidth = 10.f;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> SlashVfx;
	
	void AI_ProcessMeleeAttack();

	virtual void P_Shoot(const EPoolObject InProjectileName) override;
	virtual void P_UpdateAmmoTextHUD() override;
	void P_HandleMeleeStrike();

	virtual void BeginPlay() override;
	virtual void ShootProjectile(const EPoolObject InProjectileName, const bool bIsFromAbility, const FName InProjectileSocketName, const bool bFromFirstWeapon) override;
	void MeleeHit(AActor* InActor) const;
};
