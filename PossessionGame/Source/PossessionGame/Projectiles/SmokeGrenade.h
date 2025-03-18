#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "SmokeGrenade.generated.h"

class UNavArea;
class UNavModifierComponent;

UCLASS()
class POSSESSIONGAME_API ASmokeGrenade : public AProjectile
{
	GENERATED_BODY()
	
	ASmokeGrenade();

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UNavModifierComponent> SmokeNavModifier;
	UPROPERTY()
	TSubclassOf<UNavArea> OriginalNavAreaClass;
	
	virtual void PlayImpactSound() override;
	virtual void Init(FProjectileData& InProjectileData, const FRotator& Rotation, const FVector& Scale, const int32 InWhich) override;
	UFUNCTION()
	void OnSmokeEnd() const;
};
