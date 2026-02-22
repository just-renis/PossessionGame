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
	
	void SetXrayState(const bool bState);
	void StartTeleportProcess();
	void FinishTeleportProcess();
	void SetFirstAbilityActiveState(const bool bState) { bIsFirstAbilityActive = bState; }
private:
	bool bXrayEnabled = false;
	bool bIsFirstAbilityActive = false;
	bool bIsSecondAbilityActive = false;
	float XrayColorRadius = 3.f;
	float XrayRadialRadius = 1.f;
	float XrayDarkness = 1.f;
	int32 XrayCountLoops = 0;
	FTimerHandle ProcessXrayMaterialsHandle;
	UPROPERTY(EditDefaultsOnly, Category = "Materials", meta = (AllowPrivateAccess = true))
	TObjectPtr<UMaterialInterface> MI_Teleport;
	UPROPERTY(EditDefaultsOnly, Category = "Materials", meta = (AllowPrivateAccess = true))
	TObjectPtr<UMaterialInterface> MI_Xray;
	UPROPERTY(EditDefaultsOnly, Category = "Materials", meta = (AllowPrivateAccess = true))
	TObjectPtr<UMaterialInterface> MI_DarkXray;
	UPROPERTY(EditDefaultsOnly, Category = "Materials", meta = (AllowPrivateAccess = true))
	TObjectPtr<UMaterialInterface> MI_UnlitMaterial;
	UPROPERTY(EditDefaultsOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
	TSubclassOf<UCameraShakeBase> TeleportKillCameraShake;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> TeleportLinesVfx;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MID_Teleport;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MID_Xray;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MID_DarkXray;
	UPROPERTY()
	TObjectPtr<UMaterialInterface> MI_DefaultMaterial;
	
	virtual void AI_SetupWeapon() override;
	virtual void AI_SetController() override;
	virtual void AI_ReceivedDamage(const EDamageReason InDamageReason, const float InDamage) override;

	virtual void P_Setup() override;
	virtual void P_SetupLeftClickInputStartedState() override;
	virtual void P_SetupLeftClickInputTriggeredState() override {}
	virtual void P_WeaponShoot() override;
	virtual void P_MakeCameraShake(const ECameraShake InCameraShakeName) override;
	virtual void P_ProcessMeleeAttack() override;
	virtual void P_InitiateStomp() override;
	
	virtual void BeginPlay() override;
	void SetTeleportMaterialValue(const float InValue);
	void ProcessXrayMaterialsActivation();
	void ProcessXrayMaterialsDeactivation();
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
};