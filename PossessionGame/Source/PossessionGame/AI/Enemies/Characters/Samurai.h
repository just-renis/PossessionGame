#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "Samurai.generated.h"

class ADestructible;

UCLASS()
class POSSESSIONGAME_API ASamurai : public AEnemy
{
	GENERATED_BODY()
public:
	void AI_SlashAttack();
private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TSubclassOf<AActor> SamuraiHatClass;
	UPROPERTY()
	TObjectPtr<ADestructible> SamuraiHat;

	virtual void AI_SetupWeapon() override;
	virtual void AI_SetupShootLogic() override {}
	virtual void AI_PlayDeathElectrification() override;

	virtual void P_Setup() override;
	virtual void P_Shoot() override;
	virtual void P_BindQuickMeleeAction() override {}

	virtual void BeginPlay() override;
	void SetupSamuraiHat();
	virtual void HandleDeactivation() override;
	virtual void Landed(const FHitResult& Hit) override;
};
