#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PossessionGame/Interfaces/Interactable.h"
#include "InteractableObject.generated.h"

class UInteractableUI;
class UDelegateManager;
class UWidgetComponent;
class USphereComponent;

UCLASS()
class POSSESSIONGAME_API AInteractableObject : public AActor, public IInteractable
{
	GENERATED_BODY()
public:
	void FadeOutInteractableUI();
protected:
	AInteractableObject();

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<USphereComponent> Collision;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UWidgetComponent> InteractableWidget;
	UPROPERTY()
	TObjectPtr<UInteractableUI> InteractableUI;
	UPROPERTY()
	TObjectPtr<UDelegateManager> DelegateManager;

	virtual TObjectPtr<UStaticMeshComponent> GetMesh() { return nullptr; }
	virtual TObjectPtr<USkeletalMeshComponent> GetSkeletalMesh() { return nullptr; }
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void SetupInteractionLogic();
	virtual void Interact() override {}
private:
	FTimerHandle FadeOutAnimHandle;
	FTimerHandle DisableOutlineHandle;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	bool bIsSkeletalMesh = false;
	UPROPERTY(EditDefaultsOnly, Category = "Materials", meta = (AllowPrivateAccess = true))
	TObjectPtr<UMaterialInterface> MI_InteractionOutline;

	virtual void ChangeOutlineState(const bool bState) override;
	void FadeOutAnimFinished() const;
	void DisableOutline() { ChangeOutlineState(false); }
	UFUNCTION()
	virtual void EnableInteractionWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void DisableInteractionWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
