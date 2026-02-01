#pragma once

#include "CoreMinimal.h"
#include "OvertimeDamager.h"
#include "WraithTrapArea.generated.h"

class UNiagaraComponent;

UCLASS()
class POSSESSIONGAME_API AWraithTrapArea : public AOvertimeDamager
{
	GENERATED_BODY()

	float DefaultVfxLifetime = 5.f;
	float DefaultVfxRadius = 500.f;
	float EffectDuration = 0.f;
	UPROPERTY(EditDefaultsOnly, Category = "Collision", meta = (AllowPrivateAccess = true))
	float DefaultRadius = 500.f;
	UPROPERTY(EditDefaultsOnly, Category = "Collision", meta = (AllowPrivateAccess = true))
	float CollisionScalingOffsetSizeMultiplier = 0.84f;
	UPROPERTY(EditDefaultsOnly, Category = "Vfx", meta = (AllowPrivateAccess = true))
	float DefaultAuraRadius = 4.15f;
	UPROPERTY(EditDefaultsOnly, Category = "Vfx", meta = (AllowPrivateAccess = true))
	float DefaultFloorAuraRadius = 11.f;
	UPROPERTY(EditDefaultsOnly, Category = "Vfx", meta = (AllowPrivateAccess = true))
	float DefaultSmokeRadius = 200.f;
	UPROPERTY(EditDefaultsOnly, Category = "Vfx", meta = (AllowPrivateAccess = true))
	float DefaultSmokeCount = 50.f;
	UPROPERTY(EditDefaultsOnly, Category = "Vfx", meta = (AllowPrivateAccess = true))
	float DefaultBubbleCount = 100.f;
	UPROPERTY(EditDefaultsOnly, Category = "Vfx", meta = (AllowPrivateAccess = true))
	float DefaultGhostCount = 10.f;
	UPROPERTY(EditDefaultsOnly, Category = "Vfx", meta = (AllowPrivateAccess = true))
	float DefaultGhostRadius = 400.f;
	UPROPERTY(EditDefaultsOnly, Category = "Vfx", meta = (AllowPrivateAccess = true))
	float DefaultAuraLifetime = 4.55f;
	UPROPERTY(EditDefaultsOnly, Category = "Vfx", meta = (AllowPrivateAccess = true))
	float DefaultFloorAuraLifetime = 4.55f;
	UPROPERTY(EditDefaultsOnly, Category = "Vfx", meta = (AllowPrivateAccess = true))
	float DefaultSmokeLifetime = 3.f;
	UPROPERTY(EditDefaultsOnly, Category = "Vfx", meta = (AllowPrivateAccess = true))
	float DefaultBubbleLifetime = 3.75f;
	UPROPERTY(EditDefaultsOnly, Category = "Vfx", meta = (AllowPrivateAccess = true))
	float DefaultGhostLifetime = 3.8f;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> RadiusVfx;

	virtual void BeginPlay() override;
	virtual void DealOvertimeDamage() override;
	virtual void Setup(const bool bInIsPlayerObject, const TObjectPtr<UDamageOverTimeData>& InDamageOverTimeData, const EDamageReason InDamageReason) override;
	virtual void AddTarget(const TObjectPtr<AEnemy>& InTarget) override;
	virtual void RemoveTarget(const TObjectPtr<AEnemy>& InTarget) override;
};
