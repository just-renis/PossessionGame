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
	int32 UsbInsertedAmount = 0;
	UPROPERTY(EditAnywhere, Category = "Usb Data", meta = (AllowPrivateAccess = true))
	int32 UsbInsertableAmount = 2;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;

	virtual TObjectPtr<UStaticMeshComponent> GetMesh() override { return Mesh; }

	virtual void BeginPlay() override;
	virtual void Interact() override;
	void ShowMarker() const;
	void EnableInteraction();
	void DisableInteraction();
	void ResetTerminal();
};
