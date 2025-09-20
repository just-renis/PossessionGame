#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "GuardianProjectile.generated.h"

enum class EUpgrade : uint8;
enum class EUpgradeAttribute : uint8;

UCLASS()
class POSSESSIONGAME_API AGuardianProjectile : public AProjectile
{
	GENERATED_BODY()

	bool bIsFirstSpecialBounceUpgradeTaken = false;
	int32 CurrentBounceAmount = 0;
	UPROPERTY(EditDefaultsOnly, Category = "Vector Blast First Special Upgrade Data", meta = (AllowPrivateAccess = true))
	int32 MaxBounceAmount = 2;

	virtual void P_VariationDamageAppliance() override;

	virtual void BeginPlay() override;
	void SpecialUpgradeTaken(const EUpgrade InUpgradeName, const EUpgradeAttribute InSpecialUpgradeAttributeName);
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	void SetupBounce() const;
	virtual void DestroyThis() override;
};
