#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/PoolObject.h"
#include "Clone.generated.h"

UCLASS()
class POSSESSIONGAME_API AClone : public APoolObject
{
	GENERATED_BODY()
	
	AClone();

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> Mesh;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UAnimationAsset> WalkAnim;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UAnimMontage> ShootMontage;

	virtual void InitClone(const bool bIsSpawnedByPlayer, const TObjectPtr<USkeletalMesh> InSkeletalMesh) override;
};
