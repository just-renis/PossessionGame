#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Weapons/Categories/DualWieldingWeapon.h"
#include "MinionWeapon.generated.h"

UCLASS()
class POSSESSIONGAME_API AMinionWeapon : public ADualWieldingWeapon
{
	GENERATED_BODY()

	virtual void BeginPlay() override;
	virtual void SetupOneHandedWeapons(TObjectPtr<USkeletalMeshComponent> CharacterMesh) override;
	virtual void AttachMuzzleFlash(const EVfx InVfxName) override;
};
