#pragma once

#include "CoreMinimal.h"
#include "InteractableObject.h"
#include "UsbTerminal.generated.h"

class ACollectible;

UCLASS()
class POSSESSIONGAME_API AUsbTerminal : public AInteractableObject
{
	GENERATED_BODY()

	AUsbTerminal();

	bool bIsUsbCollected = false;
	int32 CurrentUsbIndex = 0;
	UPROPERTY(EditDefaultsOnly, Category = "Usb")
	TObjectPtr<UStaticMesh> UsbMesh;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> UsbMeshes[5];

	virtual TObjectPtr<UStaticMeshComponent> GetMesh() override { return Mesh; }

	virtual void BeginPlay() override;
	virtual void Interact() override;
	void ShowMarker() const;
	void EnableInteraction();
	void DisableInteraction();
	void ToggleInteractableWidget(const bool bState);
};
