#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Enums/EGameMode.h"
#include "PossessionGame/Interfaces/Interactable.h"
#include "Gate.generated.h"

class UInteractableUI;
class AMarker;
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
	bool bNeedsArtifact = false;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bNeededArtifact = false;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bIsUnlockableByInteraction = false;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bIsTutorialGate = false;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	EGameMode GameModeToFinish;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	EGameMode NextGameMode;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UBoxComponent> Collision;
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
	TObjectPtr<UAudioComponent> OpenGatesSfx;
	UPROPERTY()
	TObjectPtr<AMarker> Marker;
	UPROPERTY()
	FTimerHandle FadeOutAnimHandle;

	virtual bool GetIsInteractable() const override;

	virtual void BeginPlay() override;
	void OpenGate(const EGameMode InGameMode);
	void OpenGateAndShowMarker(const EGameMode InGameMode);
	void OpenAsTutorialEndGate();
	void CloseGate(const EGameMode InGameMode = EGameMode::None);
	virtual void Interact() override;
	virtual void ChangeOutlineState(const bool bState) override;
	void SetArtifactCollected() { bNeedsArtifact = false; }
	void ShowMarker(const EGameMode InGameMode);
	void FadeOutAnimFinished() const;
	UFUNCTION()
	void OpenGateSides();
	UFUNCTION()
	void CloseGateSides();
	UFUNCTION()
	void DisableOutline() { ChangeOutlineState(false); }
	UFUNCTION()
	void EnableInteractionWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void DisableInteractionWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
