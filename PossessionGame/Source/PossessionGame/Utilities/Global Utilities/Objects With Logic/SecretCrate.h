#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PossessionGame/Interfaces/Interactable.h"
#include "SecretCrate.generated.h"

class USphereComponent;
class UWidgetComponent;

UCLASS()
class POSSESSIONGAME_API ASecretCrate : public AActor, public IInteractable
{
	GENERATED_BODY()

	ASecretCrate();

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> Mesh;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UAnimSequence> OpenAnim;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UWidgetComponent> InteractableWidget;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<USphereComponent> Collision;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> RewardMesh;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UMaterialInterface> MI_InteractionOutline;
	UPROPERTY(EditDefaultsOnly, Category = "Materials", meta = (AllowPrivateAccess = true))
	TArray<TObjectPtr<UMaterialInterface>> RewardMaterials;

	virtual void BeginPlay() override;
	virtual void Interact() override;
	virtual void ChangeOutlineState(const bool bState) override;
	void GiveReward();
	
	UFUNCTION()
	void DisableOutline() { ChangeOutlineState(false); }
	UFUNCTION()
	void RewardMeshMoveUpwards() const;
	UFUNCTION()
	void StopRewardMesh();
	UFUNCTION()
	void EnableInteractionWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void DisableInteractionWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
