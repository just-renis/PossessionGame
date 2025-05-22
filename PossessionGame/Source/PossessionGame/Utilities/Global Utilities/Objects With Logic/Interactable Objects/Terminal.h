#pragma once

#include "CoreMinimal.h"
#include "InteractableObject.h"
#include "Terminal.generated.h"

class AMarker;
enum class EGameMode : uint8;
class APoolObject;

UCLASS()
class POSSESSIONGAME_API ATerminal : public AInteractableObject
{
	GENERATED_BODY()

	ATerminal();

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;
	UPROPERTY()
	TObjectPtr<AMarker> Marker;

	virtual TObjectPtr<UStaticMeshComponent> GetMesh() override { return Mesh; }
	
	virtual void BeginPlay() override;
	virtual void Interact() override;
	void ShowMarker();
	void EnableTerminal(const EGameMode InGameMode);
	void DisableTerminal();
	void ToggleInteractableWidget(const bool bState);
};
