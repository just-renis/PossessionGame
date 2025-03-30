#pragma once

#include "CoreMinimal.h"
#include "PossessionGame/Enums/EGameMode.h"
#include "PossessionGame/Interfaces/Interactable.h"
#include "Gate.generated.h"

class UWidgetComponent;
enum class EGameMode : uint8;
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
	bool bIsUnlockableByInteraction = false;
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

	virtual void BeginPlay() override;
	void OpenGate(const EGameMode InGameMode);
	void CloseGate(const EGameMode InGameMode = EGameMode::None);
	virtual void Interact() override;
	UFUNCTION()
	void OpenGateSides();
	UFUNCTION()
	void CloseGateSides();
	UFUNCTION()
	void EnableInteractionWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void DisableInteractionWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
