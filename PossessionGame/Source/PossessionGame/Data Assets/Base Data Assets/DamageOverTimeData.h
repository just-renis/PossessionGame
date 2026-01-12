#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DamageOverTimeData.generated.h"

UCLASS()
class POSSESSIONGAME_API UDamageOverTimeData : public UDataAsset
{
	GENERATED_BODY()
public:
	float GetDamageTick() const { return DamageTick; }
	float GetDamage() const { return Damage; }
	float GetAreaLifeSpan() const { return AreaLifeSpan; }
	float GetAreaSize() const { return AreaSize; }
	float GetEffectDuration() const { return EffectDuration; }
private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	float DamageTick = 0.f;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	float Damage = 0.f;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	float AreaLifeSpan = 0.f;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	float AreaSize = 0.f;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	bool bInflictsEffect = false;
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "bInflictsEffect", AllowPrivateAccess = true))
	float EffectDuration = 0.f;
};
