#pragma once

#include "CoreMinimal.h"
#include "ForceField.generated.h"

UCLASS()
class POSSESSIONGAME_API AForceField : public AActor
{
	GENERATED_BODY()

	AForceField();

	float PatternMaxOpacity = 0.25f;
	float PatternOpacityDivisor = 2.f;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	int32 ForceFieldIndex = -1;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> ForceField;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MID_Material;
	UPROPERTY()
	TObjectPtr<UAudioComponent> DissolveSfx;
	UPROPERTY()
	FTimerHandle DissolveHandle;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void DestroyThis();
	void StartDissolveForceField(const int32 InIndex);
	void ProcessDissolveForceField();
};
