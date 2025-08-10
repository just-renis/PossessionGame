#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Enums/EGameMode.h"
#include "PossessionGame/Interfaces/Interactable.h"
#include "Gate.generated.h"

class UInteractableUI;
class UWidgetComponent;
class UBoxComponent;

UCLASS()
class POSSESSIONGAME_API AGate : public AActor, public IInteractable
{
	GENERATED_BODY()

	AGate();

	bool bEventToCloseGateHappened = false;
	bool bIsGateOpened = false;
	float TimePassed = 0.f;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bIsDemoGate = false;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bNeedsArtifact = false;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bNeededArtifact = false;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bIsUnlockableByInteraction = false;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bIsTutorialBotChangeLocationGate = false;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bIsGateClosedByTrigger = false;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	EGameMode GameModeToFinish;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	EGameMode NextGameMode;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UBoxComponent> Collision;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UBoxComponent> CloseTriggerCollision;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> FirstGateSide;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> SecondGateSide;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UWidgetComponent> InteractableWidget;
	UPROPERTY(EditDefaultsOnly, Category = "Materials", meta = (AllowPrivateAccess = true))
	TObjectPtr<UMaterialInterface> MI_InteractionOutline;
	UPROPERTY()
	TObjectPtr<UInteractableUI> InteractableUI;
	UPROPERTY()
	FTimerHandle FadeOutAnimHandle;
	UPROPERTY()
	FTimerHandle OpenGatesHandle;
	UPROPERTY()
	FTimerHandle CloseGatesHandle;
	UPROPERTY()
	FTimerHandle DisableOutlineHandle;

	virtual bool GetIsInteractable() const override;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void OpenGate(const EGameMode InGameMode);
	void OpenGateAndShowMarker(const EGameMode InGameMode);
	void CloseGate(const EGameMode InGameMode = EGameMode::None);
	virtual void Interact() override;
	virtual void ChangeOutlineState(const bool bState) override;
	void SetArtifactCollected() { bNeedsArtifact = false; }
	void ShowMarker(const EGameMode InGameMode) const;
	void FadeOutAnimFinished() const;
	void OpenGateSides();
	void CloseGateSides();
	void DisableOutline() { ChangeOutlineState(false); }
	void SetupInteractability();
	UFUNCTION()
	void EnableInteractionWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void DisableInteractionWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
	void CloseTriggerPassed(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
