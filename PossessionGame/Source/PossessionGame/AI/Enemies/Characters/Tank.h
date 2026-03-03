#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/Enemy.h"
#include "Tank.generated.h"

USTRUCT()
struct FShieldPartData
{
	GENERATED_BODY()

	FShieldPartData() : Hp(0.f), MID_Default(nullptr) {}
	FShieldPartData(const float InHp, const TObjectPtr<UMaterialInstanceDynamic>& InDynamicMaterial)
	{
		Hp = InHp;
		MID_Default = InDynamicMaterial;
	}
	
	float GetHp() const { return Hp; }
	
	void TakeDamage(const float InDamage, const TObjectPtr<UWorld>& InWorld)
	{
		Hp -= InDamage;
		if (!MID_Default) return;
		MID_Default->SetScalarParameterValue(FName("Damaged"), 1.f);
		//InWorld->GetTimerManager().SetTimer(DamageTakenHandle, this, &FShieldPartData::ResetMaterialToDefault, 0.5f);
	}
	void SetHp(const float InHp) { Hp = InHp; }
private:
	FTimerHandle DamageTakenHandle;
	float Hp;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MID_Default;

	void ResetMaterialToDefault() const
	{
		if (!MID_Default) return;
		MID_Default->SetScalarParameterValue(FName("Damaged"), 0.f);
	}
};

UCLASS()
class POSSESSIONGAME_API ATank : public AEnemy
{
	GENERATED_BODY()
public:
	virtual void P_Shoot() override;
	void P_InitiateThrowGravitationalPull();
	void P_UseShield();
	void P_DisableShield();
	void P_ShieldComponentReceiveDamage(const TObjectPtr<UStaticMeshComponent>& InShieldPart, const int32 InDamage);
private:
	ATank();
	
	UPROPERTY(EditDefaultsOnly, Category = "Shield", meta = (AllowPrivateAccess = true))
	float DefaultShieldPartHp = 10.f;
	UPROPERTY(EditDefaultsOnly, Category = "Shield", meta = (AllowPrivateAccess = true))
	TArray<TObjectPtr<UStaticMesh>> ShieldMeshes;
	UPROPERTY()
	TObjectPtr<UAudioComponent> DeathSfx;
	UPROPERTY()
	TObjectPtr<UAudioComponent> SpawnSfx;
	UPROPERTY()
	TMap<TObjectPtr<UStaticMeshComponent>, FShieldPartData> ShieldParts;
	UPROPERTY()
	TMap<TObjectPtr<UStaticMeshComponent>, FTimerHandle> ShieldPartsTakeDamageHandles;
	
	virtual void AI_ReloadLogic() override;
	virtual void AI_SetCanShoot(const bool bState) override;
	virtual void AI_PlayShootMontage() override;
	virtual void AI_SetupWeapon() override;
	virtual void AI_TriggerDeathVoiceLine() override;
	virtual void AI_PlaySpawnSound() override;
	virtual void AI_PlayDeathSound() override;

	bool P_GetIsLeftOrRightInputNotPressed();
	
	virtual void P_Setup() override;
	virtual void P_SetupLeftClickInputStartedState() override;
	virtual void P_SetupLeftClickInputCompletedState() override;
	virtual void P_SetupShiftInput() override {}
	virtual void P_Destroy() override;
	void P_ToggleChargeMinigun(const bool bState);
	void P_StopChargingMinigun();
	virtual void P_WeaponShoot() override;
	UFUNCTION()
	void P_StopMinigun();
	
	virtual void BeginPlay() override;
};