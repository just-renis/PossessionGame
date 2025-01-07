#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/AI/Enemies/Management/EnemyAnimationInstance.h"
#include "RangerAnimationInstance.generated.h"

enum class EVfx : uint8;

UCLASS()
class POSSESSIONGAME_API URangerAnimationInstance : public UEnemyAnimationInstance
{
	GENERATED_BODY()

	virtual void InnerPlayMontage(EEnemyAnimState State) override;
	virtual void OnMontageEnd(UAnimMontage* Montage, bool bIsInterrupted) override;
	void SpawnOrLoadGrenadeLaunchMarker(const EVfx InVfxName, const FVector& InLandingPosition, const FRotator& InLandingRotation) const;
	void SpawnGrenadeLaunchMarker(const EVfx InVfxName, const FVector& InLandingPosition, const FRotator& InLandingRotation) const;
	UFUNCTION()
	void LaunchGrenade() const;
	UFUNCTION()
	void TriggerGrenadeProjection() const;
};
