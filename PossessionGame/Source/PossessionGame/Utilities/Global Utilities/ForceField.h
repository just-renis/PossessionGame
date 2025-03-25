#pragma once

#include "CoreMinimal.h"
#include "ForceField.generated.h"

UCLASS()
class POSSESSIONGAME_API AForceField : public AActor
{
	GENERATED_BODY()

	AForceField();

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> ForceField;

	virtual void BeginPlay() override;
	void DestroyThis();
};
