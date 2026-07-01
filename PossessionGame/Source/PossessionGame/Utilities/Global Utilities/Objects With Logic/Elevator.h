#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Elevator.generated.h"

UCLASS()
class POSSESSIONGAME_API AElevator : public AActor
{
	GENERATED_BODY()

	AElevator();

	float UnitsRose = 0.f;
	FTimerHandle RisingHandle;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float RisingSpeed = 2.f;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float UnitsToRise = 500.f;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> WardenSpawnMesh;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<AActor> WardenSpawnLocation;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UAudioComponent> ElevatorRisingSfx;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TArray<TObjectPtr<AActor>> EnemiesSpawnLocations;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void InitiateRising();
	void ProcessRising();
};
