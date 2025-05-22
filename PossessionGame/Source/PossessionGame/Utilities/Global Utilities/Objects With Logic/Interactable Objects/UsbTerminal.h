#pragma once

#include "CoreMinimal.h"
#include "InteractableObject.h"
#include "UsbTerminal.generated.h"

class AMarker;
class ACollectible;

UCLASS()
class POSSESSIONGAME_API AUsbTerminal : public AInteractableObject
{
	GENERATED_BODY()

	AUsbTerminal();

	int32 CurrentUsbIndex = 0;
	UPROPERTY(EditDefaultsOnly, Category = "Usb")
	TObjectPtr<UStaticMesh> UsbMesh;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;
	UPROPERTY()
	TObjectPtr<AMarker> Marker;
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> UsbMeshes[4];

	virtual TObjectPtr<UStaticMeshComponent> GetMesh() override { return Mesh; }

	virtual void BeginPlay() override;
	virtual void Interact() override;
	void ShowMarker();
	void EnableInteraction();
	void DisableInteraction();
	void ToggleInteractableWidget(const bool bState);
};
