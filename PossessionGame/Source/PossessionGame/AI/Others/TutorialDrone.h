#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TutorialDrone.generated.h"

class UDelegateManager;
class ATutorialDialogue;
class ADroneWaypoint;

UCLASS()
class POSSESSIONGAME_API ATutorialDrone : public AActor
{
	GENERATED_BODY()

	ATutorialDrone();

	bool bIsMoving = false;
	int32 DistanceToTargetIndex = -1;
	int32 WaypointIndex = -1;
	FVector WaypointLocation = FVector::ZeroVector;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TSubclassOf<ATutorialDialogue> DialogueClass;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UAnimationAsset> DroneAnimation;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> Mesh;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TArray<TObjectPtr<ADroneWaypoint>> Waypoints;
	UPROPERTY()
	TObjectPtr<ATutorialDialogue> Dialogue;
	UPROPERTY()
	TObjectPtr<UDelegateManager> DelegateManager;
	UPROPERTY()
	TObjectPtr<AActor> Target;

	virtual void BeginPlay() override;
	void PlayDroneAnimation() const;
	void RetrieveDroneWaypoints();
	void SetDroneMoveLocation(const int32 InIndex);
	void SetDroneLookAtLocation(const TObjectPtr<AActor>& InTarget) { Target = InTarget; }
	void EnableCheckDistanceToTarget(const int32 InIndex);
	UFUNCTION()
	void MoveToLocation();
	UFUNCTION()
	void LookAtTarget();
	UFUNCTION()
	void CheckDistanceToTarget();
};
