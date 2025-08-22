#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Enums/EBowState.h"
#include "PossessionGame/Weapons/Categories/OneHandedWeapon.h"
#include "SamuraiWeapon.generated.h"

enum class EWeaponAnimState : uint8;
enum class EHandsAnimState : uint8;
enum class EPlayerInputState : uint8;
enum class EBowState : uint8;

UCLASS()
class POSSESSIONGAME_API ASamuraiWeapon : public AOneHandedWeapon
{
	GENERATED_BODY()
public:
	EBowState GetBowState() { return BowState; }
	
	void P_SetStretchingState(EPlayerInputState bState);

	void SetBowState(EBowState bState) { BowState = bState; }
private:
	EBowState BowState = EBowState::StretchRelease;

	virtual void P_Shoot() override;
	virtual void P_UpdateAmmoTextHUD() override;
	void P_PlayBowMontage();

	virtual void BeginPlay() override;
	void PlayHandsAndWeaponMontage(EHandsAnimState State, EWeaponAnimState WeaponState, float StartSeconds = 0.f);
	virtual void ShootProjectile(const EPoolObject InProjectileName, const bool bIsFromAbility, const bool bFromFirstWeapon) override;
};
