#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PossessionGame/Interfaces/Interactable.h"
#include "Terminal.generated.h"

enum class EGameMode : uint8;
class UDelegateManager;
class APoolObject;
class USphereComponent;
class UWidgetComponent;

UCLASS()
class POSSESSIONGAME_API ATerminal : public AActor, public IInteractable
{
	GENERATED_BODY()

	ATerminal();

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<USphereComponent> Collision;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> Mesh;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UWidgetComponent> InteractableWidget;
	UPROPERTY(EditDefaultsOnly, Category = "Materials", meta = (AllowPrivateAccess = true))
	TObjectPtr<UMaterialInterface> MI_InteractionOutline;
	UPROPERTY()
	TObjectPtr<APoolObject> Marker;
	UPROPERTY()
	TObjectPtr<UDelegateManager> DelegateManager;
	
	virtual void BeginPlay() override;
	virtual void Interact() override;
	void ShowMarker();
	void EnableTerminal(const EGameMode InGameMode);
	void DisableTerminal();
	void ToggleInteractableWidget(const bool bState);
	virtual void ChangeOutlineState(const bool bState) override;
	UFUNCTION()
	void DisableOutline() { ChangeOutlineState(false); }
	UFUNCTION()
	void EnableInteractionWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void DisableInteractionWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
