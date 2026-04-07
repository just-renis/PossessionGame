#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShieldSpinner.generated.h"

UCLASS()
class POSSESSIONGAME_API AShieldSpinner : public AActor
{
	GENERATED_BODY()

	AShieldSpinner();

	bool bIsLiftingInner = false;
	float CurrentlyLiftedHeight = 0.f;
	FTimerHandle SpinHandle;
	FTimerHandle LiftingHandle;
	UPROPERTY(EditDefaultsOnly, Category = "Meshes", meta = (AllowPrivateAccess = true))
	float SecondsBetweenLiftChange = 10.f;
	UPROPERTY(EditDefaultsOnly, Category = "Meshes", meta = (AllowPrivateAccess = true))
	float LiftHeight = 100.f;
	UPROPERTY(EditDefaultsOnly, Category = "Meshes", meta = (AllowPrivateAccess = true))
	float LiftHeightStep = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Meshes", meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> InnerMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Meshes", meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> OuterMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Meshes", meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> InnerFloorMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Meshes", meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> OuterFloorMesh;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void ProcessSpin() const;
	void SetupLiftInitiationTimer();
	void SetupLifting();
	void ProcessLifting();
};
