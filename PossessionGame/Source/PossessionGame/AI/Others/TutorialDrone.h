#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TutorialDrone.generated.h"

enum class EGameMode : uint8;
enum class ESound : uint16;
class UDelegateManager;
class ATutorialDialogue;
class ADroneWaypoint;

UCLASS()
class POSSESSIONGAME_API ATutorialDrone : public AActor
{
	GENERATED_BODY()

	ATutorialDrone();

	bool bIsMoving = false;
	bool bIsRotating = false;
	bool bMoveInstantly = false;
	int32 DistanceToTargetIndex = -1;
	int32 WaypointIndex = -1;
	float RotationSpeed = 3.6f;
	float MovementSpeed = 4.5f;
	FVector WaypointLocation = FVector::ZeroVector;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TSubclassOf<ATutorialDialogue> DialogueClass;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UAnimationAsset> DroneAnimation;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> Mesh;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> FirstPropeller;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> SecondPropeller;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TArray<TObjectPtr<ADroneWaypoint>> Waypoints;
	UPROPERTY()
	TObjectPtr<ATutorialDialogue> Dialogue;
	UPROPERTY()
	TObjectPtr<UDelegateManager> DelegateManager;
	UPROPERTY()
	TObjectPtr<AActor> Target;
	UPROPERTY()
	FTimerHandle SpinPropellersHandle;
	UPROPERTY()
	FTimerHandle MoveToLocationHandle;
	UPROPERTY()
	FTimerHandle CheckDistanceToTargetHandle;
	UPROPERTY()
	FTimerHandle LookAtTargetHandle;
	UPROPERTY()
	TMap<ESound, TObjectPtr<UAudioComponent>> Sounds;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void PlayDroneAnimation() const;
	void RetrieveDroneWaypoints();
	void SetDroneMoveLocation(const int32 InIndex, const bool bInstantMove);
	void SetDroneLookAtLocation(const TObjectPtr<AActor>& InTarget) { Target = InTarget; }
	void EnableCheckDistanceToTarget(const int32 InIndex);
	void EnableCheckDistanceToTarget(const EGameMode InGameMode);
	void PlaySound(const ESound InSoundName);
	void SetupAttachments();
	void SetupDelegates();
	void ShowRandomTip() const;
	void MoveToLocation();
	void LookAtTarget();
	void CheckDistanceToTarget();
	void SpinPropellers() const;
};
