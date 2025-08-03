#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BreakableDoor.generated.h"

class USphereComponent;

UCLASS()
class POSSESSIONGAME_API ABreakableDoor : public AActor
{
	GENERATED_BODY()

	ABreakableDoor();

	bool bIsBreakable = false;
	int32 DespawnProgressIteration = 0;
	UPROPERTY(EditAnywhere, Category = "Individual Values", meta = (AllowPrivateAccess = true))
	float Force = 50.f;
	UPROPERTY(EditDefaultsOnly, Category = "Mesh", meta = (AllowPrivateAccess = true))
	TObjectPtr<USphereComponent> Collision;
	UPROPERTY(EditDefaultsOnly, Category = "Mesh", meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;
	UPROPERTY(EditDefaultsOnly, Category = "Camera Shake", meta = (AllowPrivateAccess = true))
	TSubclassOf<UCameraShakeBase> BreakDoorCameraShake;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MID;
	UPROPERTY()
	FTimerHandle PhysicsStopCheckHandle;
	UPROPERTY()
	FTimerHandle DespawnProgressHandle;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void EnableCollision() const;
	void TryBreakingDoor();
	void ProcessDespawnProgress();
	void SetupDespawnProgress();
	void CheckIfStoppedPhysics();
	UFUNCTION()
	virtual void EnableBreakable(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void DisableBreakable(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
