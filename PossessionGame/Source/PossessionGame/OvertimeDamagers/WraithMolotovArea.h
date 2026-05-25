#pragma once

#include "CoreMinimal.h"
#include "OvertimeDamager.h"
#include "WraithMolotovArea.generated.h"

class UNiagaraComponent;

UCLASS()
class POSSESSIONGAME_API AWraithMolotovArea : public AOvertimeDamager
{
	GENERATED_BODY()

	float DefaultVfxLifetime = 5.f;
	float DefaultVfxRadius = 360.f;
	UPROPERTY(EditDefaultsOnly, Category = "Collision", meta = (AllowPrivateAccess = true))
	float DefaultRadius = 360.f;
	UPROPERTY(EditDefaultsOnly, Category = "Collision", meta = (AllowPrivateAccess = true))
	float CollisionScalingOffsetSizeMultiplier = 1.1f;
	UPROPERTY(EditDefaultsOnly, Category = "Vfx", meta = (AllowPrivateAccess = true))
	float DefaultFloorAuraRadius = 11.f;
	UPROPERTY(EditDefaultsOnly, Category = "Vfx", meta = (AllowPrivateAccess = true))
	float DefaultBubblesRadius = 400.f;
	UPROPERTY(EditDefaultsOnly, Category = "Vfx", meta = (AllowPrivateAccess = true))
	float DefaultFlamesAmount = 100.f;
	UPROPERTY(EditDefaultsOnly, Category = "Vfx", meta = (AllowPrivateAccess = true))
	float DefaultAuraRadius = 4.15f;
	UPROPERTY(EditDefaultsOnly, Category = "Vfx", meta = (AllowPrivateAccess = true))
	float DefaultAuraLifetime = 4.55f;
	UPROPERTY(EditDefaultsOnly, Category = "Vfx", meta = (AllowPrivateAccess = true))
	float DefaultFlameLifetime = 3.f;
	UPROPERTY(EditDefaultsOnly, Category = "Vfx", meta = (AllowPrivateAccess = true))
	float DefaultBubblesLifetime = 4.f;
	UPROPERTY(EditDefaultsOnly, Category = "Vfx", meta = (AllowPrivateAccess = true))
	float DefaultFloorAuraLifetime = 4.55f;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> RadiusVfx;

	virtual void BeginPlay() override;
	virtual void DealOvertimeDamage() override;
	virtual void Setup(const bool bInIsPlayerObject, const TObjectPtr<UDamageOverTimeData>& InDamageOverTimeData, const EDamageReason InDamageReason) override;
};
