#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Interfaces/Collectable.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/PoolObject.h"
#include "Collectible.generated.h"

class AMarker;
class UCollectibleData;
class UNiagaraComponent;
class UDelegateManager;
class UPoolingManager;
class USphereComponent;

UCLASS()
class POSSESSIONGAME_API ACollectible : public APoolObject, public ICollectable
{
	GENERATED_BODY()

	ACollectible();

	int32 SpawnProcessIndex = 0;
	float InitialSpawnZ;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<USphereComponent> Collision;
	UPROPERTY(EditDefaultsOnly, Category = "Data", meta = (AllowPrivateAccess = true))
	TObjectPtr<UCollectibleData> Data;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> Aura;
	UPROPERTY()
	TObjectPtr<UPoolingManager> PoolingManager;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MID_Border;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MID_Main;
	UPROPERTY()
	FTimerHandle FloatHandle;
	UPROPERTY()
	FTimerHandle SpinHandle;
	UPROPERTY()
	FTimerHandle ProcessSpawnHandle;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Init(bool bInIsPlayerObject) override;
	virtual void Collect() override;
	void ToggleCollectible(const bool bState, const bool bMarkerIncluded = false);
	void Float();
	void Spin();
	void ProcessSpawnVisualization();
	void ClearSpawnVisualization();
	void EnableCollectCollision();
	UFUNCTION()
	void EnableCollectAbility(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
