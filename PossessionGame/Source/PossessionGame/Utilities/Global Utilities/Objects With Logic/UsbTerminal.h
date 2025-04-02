#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PossessionGame/Interfaces/Interactable.h"
#include "UsbTerminal.generated.h"

class ACollectible;
class UDelegateManager;
class APoolObject;
class UWidgetComponent;
class USphereComponent;

UCLASS()
class POSSESSIONGAME_API AUsbTerminal : public AActor, public IInteractable
{
	GENERATED_BODY()

	AUsbTerminal();

	int32 CurrentUsbIndex = 0;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<USphereComponent> Collision;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UWidgetComponent> InteractableWidget;
	UPROPERTY(EditDefaultsOnly, Category = "Usb")
	TObjectPtr<UStaticMesh> UsbMesh;
	UPROPERTY()
	TObjectPtr<APoolObject> Marker;
	UPROPERTY()
	TObjectPtr<UDelegateManager> DelegateManager;
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> UsbMeshes[4];

	virtual void BeginPlay() override;
	virtual void Interact() override;
	void ShowMarker();
	void EnableInteraction();
	void DisableInteraction();
	void ToggleInteractableWidget(const bool bState, const bool bMarkerIncluded = false);
	UFUNCTION()
	void EnableInteractionWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void DisableInteractionWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
