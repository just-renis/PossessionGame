#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TutorialCapsule.generated.h"

UCLASS()
class POSSESSIONGAME_API ATutorialCapsule : public AActor
{
	GENERATED_BODY()

	ATutorialCapsule();

	UPROPERTY(EditDefaultsOnly, Category = "Meshes", meta = (AllowPrivateAccess = true))
	float RotationSpeedPerSec = 30.f;
	UPROPERTY(EditDefaultsOnly, Category = "Meshes", meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> FirstRotatorMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Meshes", meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> SecondRotatorMesh;
	UPROPERTY()
	FTimerHandle RotationHandle;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void ProcessRotation() const;
};
