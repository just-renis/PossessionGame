#pragma once

#include "CoreMinimal.h"
#include "PoolObject.h"
#include "PossessionGame/Interfaces/Interactable.h"
#include "Collectible.generated.h"

class UDelegateManager;
class UPoolingManager;
class USphereComponent;
class UWidgetComponent;

UCLASS()
class POSSESSIONGAME_API ACollectible : public APoolObject, public IInteractable
{
	GENERATED_BODY()

	ACollectible();

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<USphereComponent> Collision;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UWidgetComponent> InteractableWidget;
	UPROPERTY()
	TObjectPtr<APoolObject> Marker;
	UPROPERTY()
	TObjectPtr<UPoolingManager> PoolingManager;
	UPROPERTY()
	TObjectPtr<UDelegateManager> DelegateManager;

	virtual void BeginPlay() override;
	virtual void Init(bool bInIsPlayerObject) override;
	virtual void Disable(const bool bHide) override;
	virtual void Interact() override;
	void ShowMarker();
	void EnableCollectible();
	void DisableCollectible();
	void ToggleInteractableWidget(const bool bState, const bool bMarkerIncluded = false);
	void ReturnToPool();
	UFUNCTION()
	void EnableInteractionWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void DisableInteractionWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
