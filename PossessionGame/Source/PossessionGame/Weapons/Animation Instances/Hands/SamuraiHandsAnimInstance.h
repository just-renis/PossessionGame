#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Weapons/Management/HandsAnimInstance.h"
#include "SamuraiHandsAnimInstance.generated.h"

class ASamuraiWeapon;
class UNiagaraComponent;

UCLASS()
class POSSESSIONGAME_API USamuraiHandsAnimInstance : public UHandsAnimInstance
{
	GENERATED_BODY()
public:
	void PlaySlashVfx();
private:
	UPROPERTY(EditDefaultsOnly, Category = "Melee Attack", meta = (AllowPrivateAccess = true))
	float AttackCooldown = 3.f;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> SlashVfx;
	UPROPERTY()
	TObjectPtr<ASamuraiWeapon> Weapon;

	virtual void SetProprietor(const TObjectPtr<AEnemy>& InProprietor) override;
	virtual void OnMontageEnd(UAnimMontage* InMontage, const bool bIsInterrupted) override;
	void SetAttackCooldownFinished() const;
};
