#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PossessionGame/Interfaces/Interactable.h"
#include "InteractableObject.generated.h"

class UDelegateManager;
class UWidgetComponent;
class USphereComponent;

UCLASS()
class POSSESSIONGAME_API AInteractableObject : public AActor, public IInteractable
{
	GENERATED_BODY()
protected:
	AInteractableObject();

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<USphereComponent> Collision;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UWidgetComponent> InteractableWidget;
	UPROPERTY()
	TObjectPtr<UDelegateManager> DelegateManager;

	virtual TObjectPtr<UStaticMeshComponent> GetMesh() { return nullptr; }
	virtual TObjectPtr<USkeletalMeshComponent> GetSkeletalMesh() { return nullptr; }
	
	virtual void BeginPlay() override;
	virtual void Interact() override {}
private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	bool bIsSkeletalMesh = false;
	UPROPERTY(EditDefaultsOnly, Category = "Materials", meta = (AllowPrivateAccess = true))
	TObjectPtr<UMaterialInterface> MI_InteractionOutline;

	virtual void ChangeOutlineState(const bool bState) override;
	UFUNCTION()
	void DisableOutline() { ChangeOutlineState(false); }
	UFUNCTION()
	virtual void EnableInteractionWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void DisableInteractionWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
