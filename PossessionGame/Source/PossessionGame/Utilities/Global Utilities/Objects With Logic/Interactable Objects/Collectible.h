#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Interfaces/Interactable.h"
#include "PossessionGame/Utilities/Global Utilities/Objects With Logic/PoolObject.h"
#include "Collectible.generated.h"

class UInteractableUI;
class AMarker;
class UCollectibleData;
class UNiagaraComponent;
class UDelegateManager;
class UPoolingManager;
class USphereComponent;
class UWidgetComponent;

UCLASS()
class POSSESSIONGAME_API ACollectible : public APoolObject, public IInteractable
{
	GENERATED_BODY()

	ACollectible();

	int32 SpawnProcessIndex = 0;
	float InitialSpawnZ;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<USphereComponent> Collision;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UWidgetComponent> InteractableWidget;
	UPROPERTY(EditDefaultsOnly, Category = "Materials", meta = (AllowPrivateAccess = true))
	TObjectPtr<UMaterialInterface> MI_InteractionOutline;
	UPROPERTY(EditDefaultsOnly, Category = "Data", meta = (AllowPrivateAccess = true))
	TObjectPtr<UCollectibleData> Data;
	UPROPERTY()
	TObjectPtr<UInteractableUI> InteractableUI;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> Aura;
	UPROPERTY()
	TObjectPtr<AMarker> Marker;
	UPROPERTY()
	TObjectPtr<UPoolingManager> PoolingManager;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MID_Border;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MID_Main;
	UPROPERTY()
	FTimerHandle FadeOutAnimHandle;
	UPROPERTY()
	FTimerHandle FloatHandle;
	UPROPERTY()
	FTimerHandle SpinHandle;
	UPROPERTY()
	FTimerHandle DisableOutlineHandle;
	UPROPERTY()
	FTimerHandle ProcessSpawnHandle;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Init(bool bInIsPlayerObject) override;
	virtual void Interact() override;
	void ShowMarker();
	void EnableCollectible();
	void DisableCollectible();
	void ToggleInteractableWidget(const bool bState, const bool bMarkerIncluded = false);
	virtual void ChangeOutlineState(const bool bState) override;
	void FadeOutAnimFinished() const;
	void DisableOutline() { ChangeOutlineState(false); }
	void Float();
	void Spin();
	void ProcessSpawnVisualization();
	void ClearSpawnVisualization();
	UFUNCTION()
	void EnableInteractionWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void DisableInteractionWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
