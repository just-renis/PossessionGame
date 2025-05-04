#pragma once

#include "CoreMinimal.h"
#include "InteractableObject.h"
#include "VariationsComputer.generated.h"

UCLASS()
class POSSESSIONGAME_API AVariationsComputer : public AInteractableObject
{
	GENERATED_BODY()

	AVariationsComputer();

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;

	virtual TObjectPtr<UStaticMeshComponent> GetMesh() override { return Mesh; }

	virtual void Interact() override;
};
