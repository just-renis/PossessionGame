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
	int32 TutorialForceFieldIndex = 0;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> ForceField;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MI_Material;
	UPROPERTY()
	TObjectPtr<UAudioComponent> DissolveSfx;

	virtual void BeginPlay() override;
	void DestroyThis();
	void StartDissolveTutorialForceField(const int32 InIndex);
	void StartDissolveForceField();
	UFUNCTION()
	void DissolveForceField();
};
