#pragma once

#include "CoreMinimal.h"
#include "InteractableObject.h"
#include "Terminal.generated.h"

enum class EGameMode : uint8;
class APoolObject;

UCLASS()
class POSSESSIONGAME_API ATerminal : public AInteractableObject
{
	GENERATED_BODY()

	ATerminal();

	UPROPERTY(EditAnywhere, Category = "Mode", meta = (AllowPrivateAccess = true))
	EGameMode RoomMode;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;

	virtual TObjectPtr<UStaticMeshComponent> GetMesh() override { return Mesh; }
	
	virtual void BeginPlay() override;
	virtual void Interact() override;
	void ShowMarker() const;
	void EnableTerminal(const EGameMode InGameMode);
	void DisableTerminal();
	void ToggleInteractableWidget(const bool bState);
};
