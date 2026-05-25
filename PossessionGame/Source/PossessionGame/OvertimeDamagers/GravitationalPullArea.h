#pragma once

#include "CoreMinimal.h"
#include "OvertimeDamager.h"
#include "GravitationalPullArea.generated.h"

enum class ESound : uint16;
class UNiagaraComponent;

UCLASS()
class POSSESSIONGAME_API AGravitationalPullArea : public AOvertimeDamager
{
	GENERATED_BODY()

	AGravitationalPullArea();

	UPROPERTY(EditDefaultsOnly, Category = "Legendary Explosion", meta = (AllowPrivateAccess = true))
	float DefaultLegendaryExplosionDamage = 50.f;
	UPROPERTY(EditDefaultsOnly, Category = "Legendary Explosion", meta = (AllowPrivateAccess = true))
	float KnockbackStrength = 3000.f;
	UPROPERTY()
	TObjectPtr<UAudioComponent> PassiveSfx;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> ItselfVfx;
	UPROPERTY()
	TMap<ESound, TObjectPtr<UAudioComponent>> Sounds;

	virtual void BeginPlay() override;
	virtual void DealOvertimeDamage() override;
	virtual void Disable(const bool bHide) override;
	virtual void Setup(const bool bInIsPlayerObject, const TObjectPtr<UDamageOverTimeData>& InDamageOverTimeData, const EDamageReason InDamageReason) override;
	virtual void SetupEffectStrength(const TObjectPtr<UDamageOverTimeData>& InDamageOverTimeData) override;
	void PlaySound(const ESound InSoundName);
};
