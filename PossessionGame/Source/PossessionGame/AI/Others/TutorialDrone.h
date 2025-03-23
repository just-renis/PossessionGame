#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TutorialDrone.generated.h"

class ADroneWaypoint;

UCLASS()
class POSSESSIONGAME_API ATutorialDrone : public AActor
{
	GENERATED_BODY()

	ATutorialDrone();

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UAnimationAsset> DroneAnimation;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> Mesh;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TArray<TObjectPtr<ADroneWaypoint>> Waypoints;

	virtual void BeginPlay() override;
	void PlayDroneAnimation() const;
	void RetrieveDroneWaypoints();
};
