#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PyroBossFireFloor.generated.h"

class UBoxComponent;
class UNiagaraComponent;

UCLASS()
class POSSESSIONGAME_API APyroBossFireFloor : public AActor
{
	GENERATED_BODY()

	APyroBossFireFloor();

	bool bIsWarningActive = false;
	bool bIsFireActive = false;
	float TriggerFireTime = 3.f;
	float FireUptime = 3.f;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	int32 Id;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UBoxComponent> Collision;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> EnvironmentFire;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> WarningFire;

	virtual void BeginPlay() override;
	void TriggerFireWarning(const TArray<int32>& InIds);
	void TriggerFire();
	void TurnOffFire();
	UFUNCTION()
	void AddOverlappedTarget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
