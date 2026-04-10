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
	UPROPERTY(EditAnywhere, Category = "Wire Data", meta = (AllowPrivateAccess = true))
	bool bIsWiredTerminal = false;
	UPROPERTY(EditAnywhere, Category = "Wire Data", meta = (EditCondition = "bIsWiredTerminal", EditConditionHides, AllowPrivateAccess = true))
	int32 WireId;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MID_Locks;

	virtual TObjectPtr<UStaticMeshComponent> GetMesh() override { return Mesh; }

	virtual void BeginPlay() override;
	virtual void Interact() override;
	void ShowMarker() const;
	void EnableInteraction();
	void DisableInteraction();
};
