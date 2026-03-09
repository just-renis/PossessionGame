#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Weapons/Categories/OneHandedWeapon.h"
#include "TankWeapon.generated.h"

DECLARE_DELEGATE(FOnAmmoEnded);

UCLASS()
class POSSESSIONGAME_API ATankWeapon : public AOneHandedWeapon
{
	GENERATED_BODY()
public:
	FOnAmmoEnded OnAmmoEnded;
	
	virtual void AI_Shoot() override;

	virtual void P_UpdateAmmoTextHUD() override;
	virtual void P_Shoot(const EPoolObject InProjectileName) override;
	virtual void P_Reload() override;
	virtual void P_SetupTransfer(bool bSetToFirstHand) override;
	void P_BallRelease();
private:
	int32 UpdateHandBallLoopCount = 0;
	FTimerHandle UpdateHandBallVfxHandle;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> HandBallVfx;
	
	virtual void P_Setup() override;
	void P_UpdateHandBallVfx();
	
	virtual void BeginPlay() override;
	virtual void DetachWeapon() override {}
};