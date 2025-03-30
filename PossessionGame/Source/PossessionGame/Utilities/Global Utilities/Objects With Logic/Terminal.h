#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PossessionGame/Interfaces/Interactable.h"
#include "Terminal.generated.h"

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

	virtual void BeginPlay() override;
	virtual void Interact() override;
	UFUNCTION()
	void EnableInteractionWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void DisableInteractionWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
