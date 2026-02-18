#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "Spy.generated.h"

UCLASS()
class POSSESSIONGAME_API ASpy : public AEnemy
{
	GENERATED_BODY()
public:
	bool GetXrayState() const { return bXrayEnabled; }
	
	void SetXrayState(const bool bState) { bXrayEnabled = bState; }
	void StartTeleportProcess();
	void FinishTeleportProcess();
private:
	bool bXrayEnabled = false;
	bool bIsSecondAbilityActive = false;
	UPROPERTY(EditDefaultsOnly, Category = "Materials", meta = (AllowPrivateAccess = true))
	TObjectPtr<UMaterialInterface> MI_Teleport;
	UPROPERTY(EditDefaultsOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
	TSubclassOf<UCameraShakeBase> TeleportKillCameraShake;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> TeleportLinesVfx;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MID_Teleport;
	
	virtual void AI_SetupWeapon() override;
	virtual void AI_SetController() override;

	virtual void P_Setup() override;
	virtual void P_SetupLeftClickInputStartedState() override;
	virtual void P_SetupLeftClickInputTriggeredState() override {}
	virtual void P_WeaponShoot() override;
	virtual void P_MakeCameraShake(const ECameraShake InCameraShakeName) override;
	
	virtual void BeginPlay() override;
	void SetTeleportMaterialValue(const float InValue);
};