#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Weapons/Categories/OneHandedWeapon.h"
#include "RangerWeapon.generated.h"

class UCameraComponent;

UCLASS()
class POSSESSIONGAME_API ARangerWeapon : public AOneHandedWeapon
{
	GENERATED_BODY()
public:
	ARangerWeapon();
	
	void P_ZoomToCamera(bool bIsZooming, FVector2D& MouseSensitivity);
	void ZoomLogic(TObjectPtr<UCameraComponent> Camera, float DeltaTime);
private:
	bool bIsWeaponZoomingInMotion = false;
	bool bIsZoomingIn = false;
	float ZoomStartTime = -1.f;
	float ZoomingTimer = 0.f;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Crosshair;

	virtual void AI_Reload() override;
	virtual void AI_Shoot() override;
	
	virtual void P_Shoot(bool bIsFirstWeapon) override;
	virtual void P_Reload(int32 OtherAmmoSource) override;
	
	virtual void BeginPlay() override;
	virtual void HitScanLogic() override;
};
