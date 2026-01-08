#pragma once

#include "CoreMinimal.h"
#include "OvertimeDamager.h"
#include "WraithMolotovArea.generated.h"

class UNiagaraComponent;

UCLASS()
class POSSESSIONGAME_API AWraithMolotovArea : public AOvertimeDamager
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category = "Collision", meta = (AllowPrivateAccess = true))
	float DefaultRadius = 12.f;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> RadiusVfx;

	virtual void BeginPlay() override;
	virtual void DealOvertimeDamage() override;
	virtual void Setup(const bool bInIsPlayerObject, const float InDamage, const float InDamageTick, const float InLifeSpan, const float InRadiusSize, const EDamageReason InDamageReason, const FRotator& InRotation) override;
};
