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
	
	void P_DashSlash();
	void P_EndDash();
	void P_ChangeDeflectState(const bool bState);
	void P_ThrowShuriken();
private:
	ASamurai();
	
	bool bDeflectState = false;
	bool bIsDashSlashing = false;
	bool DeflectAnimChange = false;
	FTimerHandle DashOverlapTickHandle;
	UPROPERTY(EditDefaultsOnly, Category = "Individual Values | Dash", meta = (AllowPrivateAccess = true))
	float DashStrength = 2500.f;
	UPROPERTY(EditDefaultsOnly, Category = "Individual Values | Dash", meta = (AllowPrivateAccess = true))
	float DashDuration = 0.5f;
	UPROPERTY(EditDefaultsOnly, Category = "Individual Values | Dash", meta = (AllowPrivateAccess = true))
	float DashDamage = 40.f;
	UPROPERTY(EditDefaultsOnly, Category = "Individual Values | Dash", meta = (AllowPrivateAccess = true))
	FVector DashOverlapSweepBox = FVector(150.f);
	UPROPERTY(EditDefaultsOnly, Category = "Individual Values | Dash", meta = (AllowPrivateAccess = true))
	float DashOverlapCheckTick = 0.05f;
	UPROPERTY(EditDefaultsOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
	TSubclassOf<UCameraShakeBase> SlashCameraShake;
	UPROPERTY(EditDefaultsOnly, Category = "Materials", meta = (AllowPrivateAccess = true))
	TObjectPtr<UMaterialInterface> MI_Dash;
	UPROPERTY(EditDefaultsOnly, Category = "Deflect", meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> SM_DeflectKatana;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TSubclassOf<AActor> SamuraiHatClass;
	UPROPERTY()
	TObjectPtr<ADestructible> SamuraiHat;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MID_Dash;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> DashSlashVfx;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> DashSlashLinesVfx;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> DeflectParticlesVfx;
	UPROPERTY()
	TObjectPtr<UAudioComponent> DeathSfx;
	UPROPERTY()
	TObjectPtr<UAudioComponent> SpawnSfx;
	UPROPERTY()
	TObjectPtr<UAudioComponent> DeflectSfx;
	UPROPERTY()
	TSet<TObjectPtr<AActor>> DashHitActors; 

	virtual void AI_SetupWeapon() override;
	virtual void AI_SetupShootLogic() override {}
	virtual void AI_PlayDeathElectrification() override;
	virtual void AI_TriggerDeathVoiceLine() override;
	virtual void AI_PlayDeathSound() override;
	virtual void AI_PlaySpawnSound() override;

	virtual void P_Setup() override;
	virtual void P_Shoot() override;
	virtual void P_BindQuickMeleeAction() override {}
	virtual void P_MakeCameraShake(const ECameraShake InCameraShakeName) override;
	void P_DashOverlapTick();
	void P_SetDashMaterialValue(const float InValue);

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void BeginPlay() override;
	void SetupSamuraiHat();
	virtual void HandleDeactivation() override;
	virtual void Landed(const FHitResult& Hit) override;
};
