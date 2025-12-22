#pragma once

#include "CoreMinimal.h"
#include "InteractableObject.h"
#include "PlayerLevelComputer.generated.h"

UCLASS()
class POSSESSIONGAME_API APlayerLevelComputer : public AInteractableObject
{
	GENERATED_BODY()

	APlayerLevelComputer();

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	float MarkerUpwardsUnits = 200.f;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;

	virtual TObjectPtr<UStaticMeshComponent> GetMesh() override { return Mesh; }

	virtual void BeginPlay() override;
	virtual void Interact() override;
	void SpawnMarker() const;
};
