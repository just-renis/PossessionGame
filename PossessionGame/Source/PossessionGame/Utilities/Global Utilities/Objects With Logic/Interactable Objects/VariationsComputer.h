#pragma once

#include "CoreMinimal.h"
#include "InteractableObject.h"
#include "VariationsComputer.generated.h"

enum class EWidget : uint8;

UCLASS()
class POSSESSIONGAME_API AVariationsComputer : public AInteractableObject
{
	GENERATED_BODY()

	AVariationsComputer();

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	EWidget ComputerType;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;

	virtual TObjectPtr<UStaticMeshComponent> GetMesh() override { return Mesh; }

	virtual void Interact() override;
};
