#pragma once

#include "CoreMinimal.h"
#include "ForceField.generated.h"

UCLASS()
class POSSESSIONGAME_API AForceField : public AActor
{
	GENERATED_BODY()

	AForceField();

	float MaterialOpacity = 1.f;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bIsTutorialForceField = false;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> ForceField;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MI_Material;
	UPROPERTY()
	TObjectPtr<UAudioComponent> DissolveSfx;

	virtual void BeginPlay() override;
	void DestroyThis();
	void StartDissolveForceField();
	UFUNCTION()
	void DissolveForceField();
};
